#pragma once

#include "TicketData.h"

struct KLineData;

enum KLineType { eMinute, eHour , eRaw };

class KLineDb
{
public:
	virtual ~KLineDb() {};

	virtual void Setup(char *ppInstrumentID[], int nCount) = 0;

	virtual bool Commit(TicketDataPtr spTicket) = 0;

	virtual bool Commit(const std::string& strInstrumentId, 
		                const std::string& time, 
		                const KLineData& kl, 
		                KLineType klType) = 0;
};

class KLineFileDb : public KLineDb
{
	KLineFileDb();
public:
	KLineFileDb(const KLineFileDb&)              = delete;
	KLineFileDb& operator = (const KLineFileDb&) = delete;

	virtual ~KLineFileDb();

	// Setup header for file db
	virtual void Setup(char *ppInstrumentID[], int nCount);

	virtual bool Commit(TicketDataPtr spTicket);

	virtual bool Commit(const std::string& strInstrumentId, 
		                const std::string& time, 
		                const KLineData& kl, 
		                KLineType klType);

	static KLineFileDb* Instance();

private:
	std::string GetFileDbPath(const std::string& strInstrumentId, KLineType klType) const;

private:
	std::string          m_strRootDir;
	static KLineFileDb*  s_pKLineFileDb;
};

class db
{
public:
	enum DbType {eFile, eMySql};
	static KLineDb* Get(DbType dbType);
};

