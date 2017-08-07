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
	MessageQueue(const MessageQueue& rhs)              = delete;
	MessageQueue& operator = (const MessageQueue& rhs) = delete;

	static MessageQueue* Instance();

	void Push(TicketDataPtr spTicket);
	TicketDataPtr Pop();

	bool Empty() const;

private:
	static MessageQueue*      s_pTicketQueue;
	std::queue<TicketDataPtr> m_queue;

	mutable std::mutex        m_mutex;
	std::condition_variable   m_cond;
};

