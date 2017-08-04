#pragma once

#include "TicketData.h"

#include <queue>
#include <mutex>
#include <condition_variable>

class MessageQueue
{
	MessageQueue();
public:
	~MessageQueue();
	MessageQueue(const MessageQueue& rhs) = delete;
	MessageQueue& operator = (const MessageQueue& rhs) = delete;

	static MessageQueue* Instance();

	void Push(TicketDataPtr spTicket);
	TicketDataPtr Pop();

private:
	static MessageQueue* s_pTicketQueue;
	std::queue<TicketDataPtr>	m_queue;

	std::mutex	m_pushmutex, m_popmutex;
	std::condition_variable m_cond;
};

