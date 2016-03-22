/*
	taskthread.h  
*/

#ifndef __NICK_TASKTHREAD__ 
#define __NICK_TASKTHREAD__ 

#if _MSC_VER > 1000
#pragma once
#endif 

#include "event.h"

template<class T>
class TaskThread:public Thread
{
public:
	TaskThread::TaskThread(int nThreadNumber,T * process):m_nThreadNumber(nThreadNumber),m_pProcess(process)
	{

	}
	TaskThread::~TaskThread()
	{

	}

	void Run(void * pParams)
	{
		if ( m_pProcess == NULL )
		{
			return;
		}
		while( m_bRunning == true )
		{
			m_pProcess->Working(this);
		}
	}
	bool Start()
	{
		m_Event.ReSet();
		return Thread::Start(NULL);
	}
	bool Stop()
	{
		m_Event.Set();
		return SynStop(INFINITE);
	}
	unsigned long GetThreadNumber()
	{
		return m_nThreadNumber;
	}
public:
	Event m_Event;
protected:
	T * m_pProcess ;
	unsigned long m_nThreadNumber;

};




#endif
