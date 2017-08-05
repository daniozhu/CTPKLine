#include "stdafx.h"

#include "KLineDb.h"

#include <fstream>

KLineFileDb* KLineFileDb::s_klineFileDb = nullptr;

KLineFileDb::KLineFileDb()
	:m_strRootDir("c:\\temp\\")
{

}

std::string KLineFileDb::GetFileDbPath(const std::string & strInstrumentId, KLineType klType)
{
	static std::map<std::pair<std::string, KLineType>, std::string> dbFilePathCache;

	auto key = std::make_pair(strInstrumentId, klType);
	auto iter = dbFilePathCache.find(key);
	if (iter != dbFilePathCache.end())
		return iter->second;

	std::string strFilePath = "";
	switch (klType)
	{
	case eMinute:
		strFilePath = m_strRootDir + strInstrumentId + "_Minute_KLineData.csv";
		break;
	case eHour:
		strFilePath = m_strRootDir + strInstrumentId +"_Hour_KLineData.csv";
		break;
	case eRaw:
		// for raw file, it will store all the ticket for the subscribed instruments,
		// so don't contain instrument id in the file path.
		strFilePath = m_strRootDir + "_RawData.csv";
		break;
	default:
		break;
	}

	// add it to cache 
	dbFilePathCache.emplace(key, strFilePath);

	return strFilePath;
}

KLineFileDb::~KLineFileDb()
{
}

void KLineFileDb::Setup(char *ppInstrumentID[], int nCount)
{
	// create header for raw ticket data file
	std::string strRawFilePath = GetFileDbPath("", eRaw);
	std::ofstream outFile;
	outFile.open(strRawFilePath, std::ios::out);
	outFile << "InstrumentId" << ","
			<< "LastPriceTime" << ","
			<< "LastPrice" << ","
			<< "Volume"
			<< std::endl;
	outFile.close();
	
	// create header for minute and hour k-line files.
	for (int i = 0; i < nCount; i++)
	{
		std::vector<std::string> klFilePaths;
		klFilePaths.emplace_back(GetFileDbPath(ppInstrumentID[i], eMinute));
		klFilePaths.emplace_back(GetFileDbPath(ppInstrumentID[i], eHour));
		std::for_each(klFilePaths.begin(), klFilePaths.end(), [](const std::string& path)
		{
			std::ofstream outFile;
			outFile.open(path, std::ios::out);
			outFile << "Time" << ","
				<< "OpenPrice" << ","
				<< "ClosePrice" << ","
				<< "HighPrice" << ","
				<< "LowPrice"
				<< std::endl;
			outFile.close();
		});
	}
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

//	case eMySql:
//		return KLineMySqlDb::Instance();

	default:
		return KLineFileDb::Instance();
		break;
	}
}
