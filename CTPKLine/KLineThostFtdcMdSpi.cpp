#include "stdafx.h"

#include "KLineThostFtdcMdSpi.h"
#include "MessageQueue.h"

const TThostFtdcBrokerIDType	broker_id	= "9999";
const TThostFtdcUserIDType		user_id		= "082644";
const TThostFtdcPasswordType	password	= "19820517zjh";

char* pInstrumentId[]						= {"sn1709"}; //"cu1710"
const int nInstrumentNum					= 1;

int g_request_id							= 0;

KLineThostFtdcMdSpi::KLineThostFtdcMdSpi(CThostFtdcMdApi* pUserApi)
	: m_pUserApi(pUserApi)
{
}

KLineThostFtdcMdSpi::~KLineThostFtdcMdSpi()
{
	m_pUserApi = nullptr;
}

void KLineThostFtdcMdSpi::OnFrontConnected()
{
	std::cout << "OnFrontConnected" << std::endl;

	if (!m_pUserApi)
	{
		std::cerr << "No CTP API instance created" << std::endl;
		return;
	}

	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy_s(req.BrokerID, broker_id);
	strcpy_s(req.UserID, user_id);
	strcpy_s(req.Password, password);

	int ret = m_pUserApi->ReqUserLogin(&req, g_request_id++);
	if (0 == ret)
		std::cout << "Send login request OK" << std::endl;
	else
		std::cerr << "Send login request failed with error " << ret << std::endl;
}

void KLineThostFtdcMdSpi::OnFrontDisconnected(int nReason)
{
	std::cerr << "OnFrontDisconnected due to " << nReason << std::endl;
}

void KLineThostFtdcMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField * pRspUserLogin, 
	CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	const bool bOK = (pRspInfo && 0 == pRspInfo->ErrorID);
	if (bOK)
	{
		std::cout << "Login OK" << std::endl;
		if (pRspUserLogin)
		{
			std::cout << "====================================" << std::endl;
			std::cout << "User Id: " << pRspUserLogin->UserID << std::endl;
			std::cout << "Login Time: " << pRspUserLogin->LoginTime << std::endl;
			std::cout << "Trading Day: " << pRspUserLogin->TradingDay << std::endl;
			std::cout << "System Name: " << pRspUserLogin->SystemName << std::endl;
			std::cout << "====================================" << std::endl;
		}

		// Subscribe market data for the instruments.
		if (m_pUserApi)
		{
			int ret = m_pUserApi->SubscribeMarketData(pInstrumentId, nInstrumentNum);
			if (0 == ret)
				std::cout << "Request to subscribe market data OK" << std::endl;
			else
				std::cerr << "Request to subscribe market data Failed, error: " << ret << std::endl;
		}
	}
	else
	{
		std::cerr << "Login Failed" << std::endl;
		if (pRspInfo)
			std::cerr << pRspInfo->ErrorMsg << std::endl;
	}
}

void KLineThostFtdcMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField * pSpecificInstrument,
	CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	std::cout << "OnRspSubMarketData" << std::endl;

	TThostFtdcInstrumentIDType instrument_id;
	strcpy_s(instrument_id, pSpecificInstrument ? pSpecificInstrument->InstrumentID : "unknown");

	const bool bOK = pRspInfo && (0 == pRspInfo->ErrorID);
	if (bOK)
	{
		std::cout << "Subscribe to instrument " << instrument_id << " succeeded" << std::endl;
	}
	else
	{
		std::cerr << "Subscribe to instrument " << instrument_id << " failed" << std::endl;
		if (pRspInfo)
			std::cerr << pRspInfo->ErrorMsg << std::endl;
	}
}

void KLineThostFtdcMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField * pDepthMarketData)
{
	if (pDepthMarketData)
	{
		//std::cout << "================================================="	<< std::endl;
		//std::cout << "InstrumentID: "	<< pDepthMarketData->InstrumentID	<< std::endl;
		//std::cout << "TradingDay: "		<< pDepthMarketData->TradingDay		<< std::endl;
		//std::cout << "UpdateTime: "		<< pDepthMarketData->UpdateTime		<< std::endl;
		////std::cout << "OpenPrice: "		<< pDepthMarketData->OpenPrice		<< std::endl;
		////std::cout << "ClosePrice: "		<< pDepthMarketData->ClosePrice		<< std::endl;
		////std::cout << "HighestPrice: "	<< pDepthMarketData->HighestPrice	<< std::endl;
		////std::cout << "LowestPrice: " << pDepthMarketData->LowestPrice << std::endl;
		//std::cout << "LastPrice: " << pDepthMarketData->LastPrice << std::endl;
		//std::cout << "Volume: "			<< pDepthMarketData->Volume			<< std::endl;
		//std::cout << "================================================="	<< std::endl;
		

		// Push the ticket to the queue.
		TicketDataPtr spTicket(new TicketData(pDepthMarketData));
		MessageQueue::Get()->Push(spTicket);
	}
}

void KLineThostFtdcMdSpi::OnRspError(CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	std::cout << "OnRspError" << std::endl;
	std::cerr << "RequestID: " << nRequestID << std::endl;
	std::cerr << "ErrorId: " << pRspInfo->ErrorID << std::endl;
	std::cerr << "ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;
}

void KLineThostFtdcMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	std::cout << "OnHeartBeatWarning" << std::endl;
}

void KLineThostFtdcMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField * pSpecificInstrument, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	std::cout << "OnRspUnSubMarketData" << std::endl;
}

void KLineThostFtdcMdSpi::OnRspUserLogout(CThostFtdcUserLogoutField * pUserLogout, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	std::cout << "OnRspUserLogout" << std::endl;
}

