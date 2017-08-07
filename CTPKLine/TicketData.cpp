#include "stdafx.h"

#include "TicketData.h"

//////////////////////////////////////////////////////////////////////////
// DataTime
//////////////////////////////////////////////////////////////////////////
DateTime::DateTime(const char * pDay, const char * pTime)
	: m_day(pDay), m_time(pTime)
{
	ExtractTime();
}

void DateTime::ExtractTime()
{
	if (m_time.empty())
		return;

	std::string strTime = m_time;
	size_t nPos1 = strTime.find_first_of(":");
	size_t nPos2 = strTime.find_last_of(":");

	assert(nPos1 != std::string::npos && nPos2 != std::string::npos && "invalid time format");

	m_hour = strTime.substr(0, nPos1);
	m_minute = strTime.substr(nPos1 + 1, nPos2 - nPos1 - 1);
}

//////////////////////////////////////////////////////////////////////////
// TicketData
//////////////////////////////////////////////////////////////////////////
TicketData::TicketData(const CThostFtdcDepthMarketDataField & marketData)
	: m_instrumentId(marketData.InstrumentID),
	  m_lastPriceTime(marketData.TradingDay, marketData.UpdateTime),
	  m_lastPrice(marketData.LastPrice),
	  m_volume(marketData.Volume)
{
}

