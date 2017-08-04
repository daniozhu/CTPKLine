#include "stdafx.h"
#include "MessageQueue.h"


MessageQueue* MessageQueue::g_pTicketQueue = new MessageQueue();

MessageQueue::MessageQueue()
{
}

MessageQueue::~MessageQueue()
{
}

MessageQueue * MessageQueue::Get()
{
	return g_pTicketQueue;
}

void MessageQueue::Push(TicketDataPtr spTicket)
{
	std::unique_lock<std::mutex> lock(m_pushmutex);

	m_queue.push(spTicket);
	m_cond.notify_one();
}

TicketDataPtr MessageQueue::Pop()
{
	std::unique_lock<std::mutex> lock(m_popmutex);

	if (m_queue.empty())
	{
		m_cond.wait(lock);
	}

	if(!m_queue.empty())
	{
		TicketDataPtr spTicket = m_queue.front();
		m_queue.pop();

		return spTicket;
	}
	
	return TicketDataPtr(nullptr);
}

