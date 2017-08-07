#include "stdafx.h"

#include "KLineThostFtdcMdSpi.h"
#include "MessageQueue.h"

const TThostFtdcBrokerIDType    broker_id   = "9999";
const TThostFtdcUserIDType      user_id     = "082644";
const TThostFtdcPasswordType    password    = "19820517zjh";

int g_request_id                            = 0;

char* pInstrumentId[]                       = { "sn1709", "cu1710" };
int nInstrumentNum                          = 2;

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
		std::cerr << "No CTP API instance gets created" << std::endl;
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
	                                     CThostFtdcRspInfoField * pRspInfo, 
	                                     int nRequestID, bool bIsLast)
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

		// Subscribe market data.
		if (m_pUserApi)
		{
			int ret = m_pUserApi->SubscribeMarketData(pInstrumentId, nInstrumentNum);
			if (0 == ret)
				std::cout << "Request to subscribe market data OK" << std::endl;
			else
				std::cerr << "Request to subscribe market data failed, error: " << ret << std::endl;
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
	                                         CThostFtdcRspInfoField * pRspInfo, 
	                                         int nRequestID, bool bIsLast)
{
	std::cout << "OnRspSubMarketData" << std::endl;

	std::string strInstrumentID = pSpecificInstrument ? pSpecificInstrument->InstrumentID : "unknown";

	const bool bOK = pRspInfo && (0 == pRspInfo->ErrorID);
	if (bOK)
	{
		std::cout << "Subscribe to instrument " << strInstrumentID.c_str() << " succeeded" << std::endl;
	}
	else
	{
		std::cerr << "Subscribe to instrument " << strInstrumentID.c_str() << " failed" << std::endl;
		if (pRspInfo)
			std::cerr << pRspInfo->ErrorMsg << std::endl;
	}
}

void KLineThostFtdcMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField * pDepthMarketData)
{
	if (pDepthMarketData)
	{
		assert(pDepthMarketData->LastPrice >= 0.0 && "Invalid market data");

		// Push the ticket to the message queue so we don't block the API thread too long.
		TicketDataPtr spTicket(new TicketData(*pDepthMarketData));
		MessageQueue::Instance()->Push(spTicket);
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

