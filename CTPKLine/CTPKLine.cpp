// CTPKLine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "KLineThostFtdcMdSpi.h"
#include "MessageQueue.h"

#include <thread>
#include <chrono>

//standard: tcp://180.168.146.187:10010 , 7x24: tcp://180.168.146.187:10031
char marketfront_address[] = "tcp://180.168.146.187:10031";


void ProcessTicket()
{
	while (true)
	{
		TicketDataPtr spTicket = MessageQueue::Get()->Pop();

		// Note: std::cout is not thread-safe.
		std::cout << "=================================================" << std::endl;
		std::cout << "InstrumentID: " << spTicket->InstrumentID << std::endl;
		std::cout << "TradingDay: " << spTicket->TradingDay << std::endl;
		std::cout << "UpdateTime: " << spTicket->UpdateTime << std::endl;
		std::cout << "LastPrice: " << spTicket->LastPrice << std::endl;
		std::cout << "Volume: " << spTicket->Volume << std::endl;
		std::cout << "=================================================" << std::endl;
	}
}

int main()
{
	CThostFtdcMdApi* pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();
	KLineThostFtdcMdSpi* pUserSpi = new KLineThostFtdcMdSpi(pUserApi);

	pUserApi->RegisterSpi(pUserSpi);
	pUserApi->RegisterFront(marketfront_address);
	pUserApi->Init();

	// Start another thread to process tickets from queue, e.g. save to excel & calculate KLine.
	auto processThread = std::thread(ProcessTicket);

	processThread.join();
	pUserApi->Join();

	delete pUserSpi; pUserSpi = nullptr;
	pUserApi->Release();

	return 0;
}

