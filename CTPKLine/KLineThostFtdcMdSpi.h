#pragma once
#include "../tradeapi/ThostFtdcMdApi.h"

class CThostFtdcMdApi;

class KLineThostFtdcMdSpi :
	public CThostFtdcMdSpi
{
public:
	KLineThostFtdcMdSpi(CThostFtdcMdApi* pUserApi);
	virtual ~KLineThostFtdcMdSpi();

	virtual void OnFrontConnected();
	virtual void OnFrontDisconnected(int nReason);
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, 
								CThostFtdcRspInfoField *pRspInfo, 
								int nRequestID, 
								bool bIsLast);

	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
									CThostFtdcRspInfoField *pRspInfo, 
									int nRequestID, 
									bool bIsLast);

	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnHeartBeatWarning(int nTimeLapse);

	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

private:

	CThostFtdcMdApi*			m_pUserApi;

};

