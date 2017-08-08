#pragma once

#include "TicketData.h"

#include <queue>
#include <mutex>
#include <condition_variable>

template <class T>
class MessageQueue
{
	MessageQueue();
public:
	~MessageQueue();
	MessageQueue(const MessageQueue& rhs)              = delete;
	MessageQueue& operator = (const MessageQueue& rhs) = delete;

	static MessageQueue* Instance();

	void Push(const T& item);
	T Pop();

	bool Empty() const;

private:
	static MessageQueue<T>*      s_pTicketQueue;
	std::queue<T> m_queue;

	mutable std::mutex           m_mutex;
	std::condition_variable      m_cond;
};

template <class T>
MessageQueue<T>* MessageQueue<T>::s_pTicketQueue = new MessageQueue<T>();

template<class T>
MessageQueue<T>::MessageQueue()
{}

template<class T>
MessageQueue<T>::~MessageQueue()
{}

template<class T>
MessageQueue<T>* MessageQueue<T>::Instance()
{
	return s_pTicketQueue;
}

template<class T>
void MessageQueue<T>::Push(const T& item)
{
	std::unique_lock<std::mutex> lck(m_mutex);

	m_queue.push(item);

	// Unlock the mutex so waiting thread can acquire it immediately when get notified.
	lck.unlock();
	m_cond.notify_one();
}

template<class T>
T MessageQueue<T>::Pop()
{
	std::unique_lock<std::mutex> lck(m_mutex);
	while (m_queue.empty())
	{
		m_cond.wait(lck);
	}

	T item = m_queue.front();
	m_queue.pop();

	return item;
}

template<class T>
bool MessageQueue<T>::Empty() const
{
	std::unique_lock<std::mutex> lck(m_mutex);
	return m_queue.empty();
}
