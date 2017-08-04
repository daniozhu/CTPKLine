#include "stdafx.h"

#include "db.h"

#include <fstream>

KLineFileDb* KLineFileDb::s_klineFileDb = nullptr;

KLineFileDb::KLineFileDb()
	:m_strRootDir("c:\\temp\\")
{

}

std::string KLineFileDb::GetFileDbPath(const std::string & strInstrumentId, KLineType klType)
{
	std::string strType = "";
	switch (klType)
	{
	case eMinute:
		strType = "_Minute_KLineData.csv";
		break;
	case eHour:
		strType = "_Hour_KLineData.csv";
		break;
	case eRaw:
		strType = "_RawData.csv";
		// for raw file, it will store all the ticket for the subscribed instruments,
		// so don't contain instrument id in the file path.
		return m_strRootDir + strType;
		break;
	default:
		break;
	}

	return m_strRootDir + strInstrumentId + strType;
}

KLineFileDb::~KLineFileDb()
{
}

void KLineFileDb::Init(const std::string & strInstrumentId, KLineType klType)
{
	std::string filePath = GetFileDbPath(strInstrumentId, klType);

	std::ofstream outFile;
	outFile.open(filePath, std::ios::out);

	if (klType == eRaw)
	{
		outFile << "InstrumentId" << ","
			<< "LastPriceTime" << ","
			<< "LastPrice" << ","
			<< "Volume"
			<< std::endl;
	}
	else
	{
		outFile <<"Time" << ","  
			<<"OpenPrice" << ","
			<< "ClosePrice" << ","
			<< "HighPrice" << ","
			<< "LowPrice"
			<< std::endl;
	}

	outFile.close();
}

// For raw data
bool KLineFileDb::Commit(TicketDataPtr spTicket)
{
	std::string filePath = GetFileDbPath(spTicket->InstrumentID, eRaw);

	std::ofstream outFile;
	outFile.open(filePath, std::ios::app);
	outFile << spTicket->InstrumentID << ","
		<< spTicket->LastPriceTime.Day().c_str() <<" "<<spTicket->LastPriceTime.Time().c_str() << ","
		<< spTicket->LastPrice << ", "
		<< spTicket->Volume
		<< std::endl;
	outFile.close();

	return true;
}

// For K-line data
bool KLineFileDb::Commit(const std::string & strInstrumentId,
	const DateTime & time, const KLineData & kl, KLineType klType)
{
	std::string filePath = GetFileDbPath(strInstrumentId, klType);

	std::ofstream outFile;
	outFile.open(filePath, std::ios::app); 
	outFile << time.Day().c_str() <<" " << time.Time().c_str() <<","
		<<kl.OpenPrice << ","
		<< kl.ClosePrice << ","
		<< kl.HighPrice << ","
		<< kl.LowPrice
		<< std::endl;
	outFile.close();

	return true;
}

KLineFileDb* KLineFileDb::Instance()
{
	if (!s_klineFileDb)
		s_klineFileDb = new KLineFileDb();

	return s_klineFileDb;
}

KLineDb * db::Get(DbType dbType)
{
	switch (dbType)
	{
	case eFile:
		return KLineFileDb::Instance();
		break;

	case eMySql:
		return nullptr;
		break;

	default:
		return nullptr;
		break;
	}
}
