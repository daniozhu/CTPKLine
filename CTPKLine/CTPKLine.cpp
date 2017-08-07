// CTPKLine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "KLineThostFtdcMdSpi.h"
#include "MessageQueue.h"
#include "KLineDb.h"

#include <thread>

// standard: tcp://180.168.146.187:10010
// 7x24:     tcp://180.168.146.187:10031
char* marketfront_address = "tcp://180.168.146.187:10010";

bool bContinueToCreateKLine = true;

typedef std::pair<DateTime /*update time*/, double/*last price*/> TimePrice;
typedef std::vector<TimePrice> TimePriceVec;

extern char* pInstrumentId[];
extern int nInstrumentNum;

bool TimePriceComp(const TimePrice& tp1, const TimePrice& tp2)
{
	// Compare price but not time.
	return tp1.second < tp2.second;
};

void CreateMinuteKLine(const std::string strIntrumentId, const TimePriceVec& timePriceVec)
{
	assert(!timePriceVec.empty());

	const DateTime& prePriceTime = timePriceVec.back().first;

	// Find the first item from the vector where need start to calculate the minute k-line.
	auto iter = std::find_if(timePriceVec.begin(), timePriceVec.end(),
		[&prePriceTime](const TimePrice& tp)
	{
		return (tp.first.Hour().compare(prePriceTime.Hour()) == 0) &&
			(tp.first.Minute().compare(prePriceTime.Minute()) == 0);
	});

	assert(iter != timePriceVec.end());

	KLineData klData;
	klData.OpenPrice  = iter->second;
	klData.ClosePrice = timePriceVec.back().second;
	klData.HighPrice  = std::max_element(iter, timePriceVec.end(), TimePriceComp)->second;
	klData.LowPrice   = std::min_element(iter, timePriceVec.end(), TimePriceComp)->second;

	std::string strTime = prePriceTime.Hour() + ":" + prePriceTime.Minute() + ":00";
	db::Get(db::eFile)->Commit(strIntrumentId, strTime, klData, KLineType::eMinute);

	std::cout << "****** Created minute k-line*******" << std::endl;
	std::cout << "InstrumentId: " << strIntrumentId.c_str() << ", Time: " << strTime.c_str() << std::endl;
	std::cout << "************************************" << std::endl;
}

void CreateHourKLine(const std::string strIntrumentId, const TimePriceVec& timePriceVec)
{
	assert(!timePriceVec.empty());

	// Get the time for the previous ticket.
	const DateTime& prePriceTime = timePriceVec.back().first;

	// calculate and save hour k-line
	KLineData klData;
	klData.OpenPrice  = timePriceVec.front().second;
	klData.ClosePrice = timePriceVec.back().second;
	klData.HighPrice  = std::max_element(timePriceVec.begin(), timePriceVec.end(), TimePriceComp)->second;
	klData.LowPrice   = std::min_element(timePriceVec.begin(), timePriceVec.end(), TimePriceComp)->second;

	std::string strTime = prePriceTime.Hour() + ":00:00";
	db::Get(db::eFile)->Commit(strIntrumentId, strTime, klData, KLineType::eHour);

	std::cout << "****** Created hour k-line*********" << std::endl;
	std::cout << "InstrumentId: " << strIntrumentId.c_str() << ", Time: " << strTime.c_str() << std::endl;
	std::cout << "************************************" << std::endl;
}

void TicketToKLine()
{
	std::map<std::string, TimePriceVec> instrumentPriceMap;

	while (bContinueToCreateKLine)
	{
		TicketDataPtr spTicket = MessageQueue::Instance()->Pop();
		if(spTicket.get() == nullptr)
			continue;

		// Note: std::cout is not thread-safe.
		std::cout << "=================================================" << std::endl;
		std::cout << "InstrumentID: " << spTicket->InstrumentID().c_str() << std::endl;
		std::cout << "TradingDay: " << spTicket->LastPriceTime().Day().c_str() << std::endl;
		std::cout << "UpdateTime: " << spTicket->LastPriceTime().Time().c_str() << std::endl;
		std::cout << "LastPrice: " << spTicket->LastPrice() << std::endl;
		std::cout << "Volume: " << spTicket->Volume() << std::endl;
		std::cout << "=================================================" << std::endl;

		// Write the raw market data to file db.
		db::Get(db::eFile)->Commit(spTicket);

		const std::string& strIntrumentId = spTicket->InstrumentID();
		auto iter = instrumentPriceMap.find(strIntrumentId);
		
		// This is the first ticket for the instrument we start to collect.
		if (iter == instrumentPriceMap.end())
		{
			TimePriceVec priceVec;
			priceVec.emplace_back(spTicket->LastPriceTime(), spTicket->LastPrice());
			
			instrumentPriceMap.emplace(strIntrumentId, priceVec);
			continue;
		}

		TimePriceVec& timePriceVec = iter->second;
		const DateTime& prePriceTime = timePriceVec.back().first;

		bool bDifferentHour = prePriceTime.Hour().compare(spTicket->LastPriceTime().Hour()) != 0;
		bool bDifferentMinute = prePriceTime.Minute().compare(spTicket->LastPriceTime().Minute()) != 0;

		if (bDifferentMinute)
		{
			CreateMinuteKLine(strIntrumentId, timePriceVec);
		}

		if (bDifferentHour)
		{
			CreateHourKLine(strIntrumentId, timePriceVec);

			// clear the vector to be used for next hour k-line.
			timePriceVec.clear();
		}

		// add the new ticket to the vector.
		timePriceVec.emplace_back(spTicket->LastPriceTime(), spTicket->LastPrice());
	}
}

int main()
{
	CThostFtdcMdApi* pUserApi     = CThostFtdcMdApi::CreateFtdcMdApi();
	KLineThostFtdcMdSpi* pUserSpi = new KLineThostFtdcMdSpi(pUserApi);

	pUserApi->RegisterSpi(pUserSpi);
	pUserApi->RegisterFront(marketfront_address);
	pUserApi->Init();

	// Setup file db, e.g. create headers
	db::Get(db::eFile)->Setup(pInstrumentId, nInstrumentNum);

	// Start a thread to create k-line.
	auto processThread = std::thread(TicketToKLine);

	pUserApi->Join();

	bContinueToCreateKLine = false;
	processThread.join();

	delete pUserSpi; pUserSpi = nullptr;
	pUserApi->Release();

	// clean up global/singleton obj
	delete MessageQueue::Instance();
	delete db::Get(db::eFile);

	return 0;
}

