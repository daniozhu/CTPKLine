// CTPKLine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "KLineThostFtdcMdSpi.h"
#include "MessageQueue.h"
#include "KLineDb.h"

#include <thread>

//standard: tcp://180.168.146.187:10010 , 7x24: tcp://180.168.146.187:10031
char* marketfront_address = "tcp://180.168.146.187:10031";

bool bContinueProcess = true;

typedef std::pair<DateTime /*update time*/, double/*last price*/> TimePrice;
typedef std::vector<TimePrice> TimePriceVec;

bool TimePriceComp(const TimePrice& tp1, const TimePrice& tp2)
{
	// Compare price but not time.
	return tp1.second < tp2.second;
};

void TicketToKLine()
{
	std::map<std::string, TimePriceVec> instrumentPriceMap;

	while (bContinueProcess)
	{
		TicketDataPtr spTicket = MessageQueue::Instance()->Pop();
		if(spTicket.get() == nullptr)
			continue;

		// Note: std::cout is not thread-safe.
		std::cout << "=================================================" << std::endl;
		std::cout << "InstrumentID: " << spTicket->InstrumentID << std::endl;
		std::cout << "TradingDay: " << spTicket->LastPriceTime.Day().c_str() << std::endl;
		std::cout << "UpdateTime: " << spTicket->LastPriceTime.Time().c_str() << std::endl;
		std::cout << "LastPrice: " << spTicket->LastPrice << std::endl;
		std::cout << "Volume: " << spTicket->Volume << std::endl;
		std::cout << "=================================================" << std::endl;

		// write the raw ticket data to file db.
		db::Get(db::eFile)->Commit(spTicket);

		std::string strIntrumentId = spTicket->InstrumentID;
		auto iter = instrumentPriceMap.find(strIntrumentId);
		
		// Process the first ticket for the instrument
		if (iter == instrumentPriceMap.end())
		{
			TimePriceVec priceVec;
			priceVec.emplace_back(spTicket->LastPriceTime, spTicket->LastPrice);
			
			instrumentPriceMap.emplace(strIntrumentId, priceVec);
			continue;
		}

		TimePriceVec& timePriceVec = iter->second;
		DateTime prePriceTime = timePriceVec.back().first;
		
		const std::string& strPreHour = prePriceTime.Hour();
		const std::string& strLastHour = spTicket->LastPriceTime.Hour();
		const std::string& strPreMinute = prePriceTime.Minute();
		const std::string& strLastMinute = spTicket->LastPriceTime.Minute();

		bool bSamehour = strPreHour.compare(strLastHour) == 0;
		bool bSameMinute = strPreMinute.compare(strLastMinute) == 0;

		if (bSamehour && !bSameMinute)
		{
			// Find the first item where need start to calculate the minute k-line
			auto iter = std::find_if(timePriceVec.begin(), timePriceVec.end(),
				[&prePriceTime](const TimePrice& tp) 
			{
				return  (tp.first.Hour().compare(prePriceTime.Hour()) == 0) &&
						(tp.first.Minute().compare(prePriceTime.Minute()) == 0);
			});

			KLineData klData;
			klData.OpenPrice = iter->second;
			klData.ClosePrice = timePriceVec.back().second;
			klData.HighPrice = std::max_element(iter, timePriceVec.end(), TimePriceComp)->second;
			klData.LowPrice = std::min_element(iter, timePriceVec.end(), TimePriceComp)->second;

			std::string strTime = prePriceTime.Hour() + ":" + prePriceTime.Minute() + ":00";
			DateTime klTime(prePriceTime.Day().c_str(), strTime.c_str());

			db::Get(db::eFile)->Commit(strIntrumentId, klTime, klData, KLineType::eMinute);
		}
		else if (!bSamehour)
		{
			// calculate and save hour k-line
			KLineData klData;
			klData.OpenPrice = timePriceVec.front().second;
			klData.ClosePrice = timePriceVec.back().second;
			klData.HighPrice = std::max_element(timePriceVec.begin(), timePriceVec.end(), TimePriceComp)->second;
			klData.LowPrice = std::min_element(timePriceVec.begin(), timePriceVec.end(), TimePriceComp)->second;

			std::string strTime = prePriceTime.Hour() + ":00:00";
			DateTime klTime(prePriceTime.Day().c_str(), strTime.c_str());

			db::Get(db::eFile)->Commit(strIntrumentId, klTime, klData, KLineType::eHour);

			// clear the vector and be used for next hour.
			timePriceVec.clear();
		}

		timePriceVec.emplace_back(spTicket->LastPriceTime, spTicket->LastPrice);
	}
}

int main()
{
	CThostFtdcMdApi* pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();
	KLineThostFtdcMdSpi* pUserSpi = new KLineThostFtdcMdSpi(pUserApi);

	pUserApi->RegisterSpi(pUserSpi);
	pUserApi->RegisterFront(marketfront_address);
	pUserApi->Init();

	auto processThread = std::thread(TicketToKLine);

	processThread.join();
	pUserApi->Join();

	delete pUserSpi; pUserSpi = nullptr;
	pUserApi->Release();

	// clean up global/singleton obj
	delete MessageQueue::Instance();
	delete db::Get(db::eFile);

	return 0;
}

