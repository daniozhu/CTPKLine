#include "stdafx.h"
#include "MessageQueue.h"


MessageQueue* MessageQueue::s_pTicketQueue = new MessageQueue();

MessageQueue::MessageQueue()
{
}

MessageQueue::~MessageQueue()
{
}

MessageQueue * MessageQueue::Instance()
{
	return s_pTicketQueue;
}

void MessageQueue::Push(TicketDataPtr spTicket)
{
	std::unique_lock<std::mutex> lck(m_mutex);

	m_queue.push(spTicket);

	// Unlock the mutex so waiting thread can acquire it immediately when get notified.
	lck.unlock();
	m_cond.notify_one();
}

TicketDataPtr MessageQueue::Pop()
{
	std::unique_lock<std::mutex> lck(m_mutex);

	while (m_queue.empty())
	{
		m_cond.wait(lck);
	}

	TicketDataPtr spTicket = m_queue.front();
	m_queue.pop();

	return spTicket;
}

bool MessageQueue::Empty() const
{
	std::unique_lock<std::mutex> lck(m_mutex);

	return m_queue.empty();
}

