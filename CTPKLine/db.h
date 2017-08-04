#pragma once

#include "TicketData.h"

struct KLineData;

enum KLineType { eMinute, eHour , eRaw };

class KLineDb
{
public:
	virtual ~KLineDb() {};

	virtual void Init(const std::string& strInstrumentId, KLineType klType) = 0;

	virtual bool Commit(TicketDataPtr spTicket) = 0;

	virtual bool Commit(const std::string& strInstrumentId,
		const DateTime& time, const KLineData& kl, KLineType klType) = 0;
};

class KLineFileDb : public KLineDb
{
public:
	virtual ~KLineFileDb();

	// Setup header
	virtual void Init(const std::string& strInstrumentId, KLineType klType);

	virtual bool Commit(TicketDataPtr spTicket);

	virtual bool Commit(const std::string& strInstrumentId, 
		const DateTime& time, const KLineData& kl, KLineType klType);

	static KLineFileDb* Instance();

private:
	KLineFileDb();
	KLineFileDb(const KLineFileDb&);

	std::string GetFileDbPath(const std::string& strInstrumentId, KLineType klType);

private:
	std::string m_strRootDir;
	static KLineFileDb* s_klineFileDb;
};

class db
{
public:
	enum DbType {eFile, eMySql};
	static KLineDb* Get(DbType dbType);

private:
	static KLineFileDb* s_pKLineFileDb;
};
