#pragma once

#include "../tradeapi/ThostFtdcUserApiDataType.h"
#include "../tradeapi/ThostFtdcUserApiStruct.h"

#include <memory>

struct CThostFtdcDepthMarketDataField;

class DateTime
{
public:
	DateTime()
	{}

	DateTime(const char* pDay, const char* pTime)
		:m_day(pDay), m_time(pTime)
	{
		ExtractTime();
	}

	DateTime(const DateTime& rhs)
	{
		m_day = rhs.m_day;
		m_time = rhs.m_time;
		m_hour = rhs.m_hour;
		m_minute = rhs.m_minute;
	}

	DateTime& operator = (const DateTime& rhs)
	{
		if (&rhs == this)
			return *this;

		m_day = rhs.m_day;
		m_time = rhs.m_time;
		m_hour = rhs.m_hour;
		m_minute = rhs.m_minute;

		return *this;
	}
	
	void SetDay(const char* pDay)
	{
		m_day = pDay;
	}

	void SetTime(const char* pTime)
	{
		m_time = pTime;

		ExtractTime();
	}

	const std::string& Day() const 
	{
		return m_day;
	}

	const std::string& Time() const
	{
		return m_time;
	}

	const std::string& Hour() const
	{
		return m_hour;
	}
	
	const std::string& Minute() const
	{
		return m_minute;
	}

private:
	void ExtractTime()
	{
		std::string strTime = m_time;
		size_t nPos = strTime.find_first_of(":");
		size_t nPos2 = strTime.find_last_of(":");

		m_hour = strTime.substr(0, nPos);
		m_minute = strTime.substr(nPos+1, nPos2-nPos-1);
	}

private:
	std::string m_day;
	std::string m_time;
	std::string m_hour;
	std::string m_minute;
};

struct TicketData
{
	TicketData(const CThostFtdcDepthMarketDataField* pMarketData)
		: LastPrice(0.0), Volume(0)
	{
		memset(InstrumentID, 0, sizeof(InstrumentID));

		if (pMarketData)
		{
			strcpy_s(InstrumentID, pMarketData->InstrumentID);

			LastPriceTime.SetDay(pMarketData->TradingDay);
			LastPriceTime.SetTime(pMarketData->UpdateTime);

			LastPrice = pMarketData->LastPrice;
			Volume = pMarketData->Volume;
		}
	}

	TThostFtdcInstrumentIDType InstrumentID;
	DateTime LastPriceTime;
	TThostFtdcPriceType LastPrice;
	TThostFtdcVolumeType Volume;
};

typedef std::shared_ptr<TicketData> TicketDataPtr;

struct KLineData
{
	TThostFtdcPriceType OpenPrice;
	TThostFtdcPriceType HighPrice;
	TThostFtdcPriceType LowPrice;
	TThostFtdcPriceType ClosePrice;
};