/*
	safequeue.h  
*/

#ifndef __NICK_SAFEQUEUE__ 
#define __NICK_SAFEQUEUE__ 

#if _MSC_VER > 1000
#pragma once
#endif 

#ifndef SEM_VALUE_MAX
#define SEM_VALUE_MAX  INT_MAX
#endif

#include "semaphore.h"


#include <queue>

namespace COMMON
{

enum
{
	DEFAULT_SIZE = 1024*20,
	MAX_SIZE	= 4000*1024
};
template<class T> class SafeQueue
{
public:
	SafeQueue(int nQueueSize = DEFAULT_SIZE);
	virtual ~SafeQueue();
	bool Push(const T & Value);	
	bool BlockPush(const T & Value);
	bool BlockPop(T &ret_value);
	bool Pop(T &ret_value,int nTimeOut);

	int GetQueueSize();
	void Clear();
protected:
	unsigned long m_nQueueSize ;
	Semaphore m_semaEmputy;
	Semaphore m_semaFull;
	Mutex m_lock;
	std::queue<T> m_queue;
};
template<class T>
SafeQueue<T>::SafeQueue(int nQueueSize):
#ifdef WIN32
m_nQueueSize(nQueueSize > MAX_SIZE ? MAX_SIZE : nQueueSize), 
#else
m_nQueueSize(nQueueSize > SEM_VALUE_MAX ? SEM_VALUE_MAX : nQueueSize), 
#endif
m_semaEmputy(nQueueSize,nQueueSize),
m_semaFull(0,nQueueSize)
{
	
}

template<class T>
SafeQueue<T>::~SafeQueue()
{

}

template<class T>
bool SafeQueue<T>::Push(const T & value)
{
	SingleMutex sMutex(&m_lock);
	if ( m_queue.size() >= m_nQueueSize )
	{
		return false;
	}
	m_semaEmputy.Wait();
	m_queue.push(value);
	m_semaFull.Post();
	return true ;
}
template<class T>
bool SafeQueue<T>::BlockPush(const T & value)
{
	m_semaEmputy.Wait()
	{
		SingleMutex sMutex(m_lock);
		m_queue.push(value);
		m_semaFull.Post();
	}
	return true ;
}
template<class T>
bool SafeQueue<T>::BlockPop(T &ret_value)
{
	m_semaFull.Wait()
	{
		SingleMutex sMutex(m_lock);
		ret_value = m_queue.front();
		m_queue.pop();
		m_semaEmputy.Post();
	}
	return true;
}
template<class T>
bool SafeQueue<T>::Pop(T &ret_value,int nTimeOut)
{
	if ( true == m_semaFull.Wait(nTimeOut))
	{
		SingleMutex sMutex(&m_lock);
		ret_value = m_queue.front();
		m_queue.pop();
		m_semaEmputy.Post();
		return true ;
	}
	else
	{
		return false;
	}
}
template<class T>
int SafeQueue<T>::GetQueueSize()
{
	SingleMutex sMutex(&m_lock);
	return m_queue,size();
}

template<class T>
void SafeQueue<T>::Clear()
{
	SingleMutex sMutex(&m_lock);

	while(!m_queue.empty())
	{
		m_queue.pop();	
		m_semaFull.Wait();
		m_semaEmputy.Post();
	}
}



}



#endif