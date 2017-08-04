#pragma once

#include "../tradeapi/ThostFtdcUserApiDataType.h"
#include "../tradeapi/ThostFtdcUserApiStruct.h"

struct CThostFtdcDepthMarketDataField;

struct TicketData
{
	TicketData(const CThostFtdcDepthMarketDataField* pMarketData) :
		LastPrice(0.0), Volume(0)
	{
		memset(InstrumentID, 0, sizeof(InstrumentID));
		memset(TradingDay, 0, sizeof(TradingDay));
		memset(UpdateTime, 0, sizeof(UpdateTime));

		if (pMarketData)
		{
			strcpy_s(InstrumentID, pMarketData->InstrumentID);
			strcpy_s(TradingDay, pMarketData->TradingDay);
			strcpy_s(UpdateTime, pMarketData->UpdateTime);

			LastPrice = pMarketData->LastPrice;
			Volume = pMarketData->Volume;
		}
	}

	TThostFtdcInstrumentIDType	InstrumentID;
	TThostFtdcDateType	TradingDay;
	TThostFtdcTimeType	UpdateTime;
	TThostFtdcPriceType	LastPrice;
	TThostFtdcVolumeType Volume;
};