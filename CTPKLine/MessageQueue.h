#pragma once

#include "TicketData.h"

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

typedef std::shared_ptr<TicketData> TicketDataPtr;

class MessageQueue
{
	MessageQueue();
public:
	~MessageQueue();
	MessageQueue(const MessageQueue& rhs) = delete;
	MessageQueue& operator = (const MessageQueue& rhs) = delete;

	static MessageQueue* Get();

	void Push(TicketDataPtr spTicket);
	TicketDataPtr Pop();

private:
	static MessageQueue* g_pTicketQueue;
	std::queue<TicketDataPtr>	m_queue;

	std::mutex	m_pushmutex, m_popmutex;
	std::condition_variable m_cond;
};

