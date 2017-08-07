#pragma once

#include "../tradeapi/ThostFtdcUserApiDataType.h"
#include "../tradeapi/ThostFtdcUserApiStruct.h"

#include <memory>
#include <assert.h>

class DateTime
{
public:
	DateTime(const char* pDay, const char* pTime);
	
	const std::string& Day() const {
		return m_day;
	}

	const std::string& Time() const {
		return m_time;
	}

	const std::string& Hour() const {
		return m_hour;
	}
	
	const std::string& Minute() const {
		return m_minute;
	}

private:
	void ExtractTime();

private:
	std::string m_day;
	std::string m_time;
	std::string m_hour;
	std::string m_minute;
};

class TicketData
{
public:
	TicketData(const CThostFtdcDepthMarketDataField& marketData);

	const std::string& InstrumentID() const {
		return m_instrumentId;
	}

	const DateTime& LastPriceTime() const {
		return m_lastPriceTime;
	}

	double LastPrice() const {
		return m_lastPrice;
	}

	int Volume() const {
		return m_volume;
	}

private:
	std::string   m_instrumentId;
	DateTime      m_lastPriceTime;
	double        m_lastPrice;
	int           m_volume;
};
typedef std::shared_ptr<TicketData> TicketDataPtr;

struct KLineData
{
	TThostFtdcPriceType OpenPrice;
	TThostFtdcPriceType HighPrice;
	TThostFtdcPriceType LowPrice;
	TThostFtdcPriceType ClosePrice;
};