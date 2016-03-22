
/*
	semaphore.h  
*/

#ifndef __NICK_SEMAPHORE__ 
#define __NICK_SEMAPHORE__ 

#if _MSC_VER > 1000
#pragma once
#endif 


#ifndef WIN32
#include "semaphore/semaphore.h"
#endif

namespace COMMON
{

class Semaphore
{
public:
	Semaphore(unsigned long nInit,unsigned long nMax);
	virtual ~Semaphore();
	bool Post();
	bool Wait();
	bool Wait(int nTimeOut/*nMilliseconds*/);
private:
#ifdef WIN32
	HANDLE m_semaphore;
#elif defined __IOS__
	semaphore_t m_Semaphore;
#else
	sem_t m_Semaphore;
#endif

};
Semaphore::Semaphore(unsigned long nInit,unsigned long nMax)
{
#ifdef WIN32
	m_semaphore = CreateSemaphore(NULL,nInit,nMax,NULL);
#elif defined __IOS__
	semaphore_create(mach_task_self(),&m_Semaphore,SYNC_POLICY_FIFO,initCount);
#else
	sem_init(&m_Semaphore, 0, initCount);
#endif
}
Semaphore::~Semaphore()
{
#ifdef WIN32
	CloseHandle(m_semaphore);
	m_semaphore = NULL;
#elif defined __IOS__
	semaphore_destroy(mach_task_self(),m_Semaphore);
#else
	sem_destroy(&m_Semaphore);
#endif
}


bool Semaphore::Post() // 给 指定的信号量 ++
{
	if ( m_semaphore == 0 )
	{
		return false;
	}
#ifdef WIN32
	return ReleaseSemaphore(m_semaphore,1,NULL);
#elif defined __IOS__
	semaphore_signal(m_Semaphore);
#else
	if ( sem_post(&m_Semaphore) == 0 )
		return true;
	else
		return false;
#endif
}
bool Semaphore::Wait()
{
	if ( m_semaphore == 0 )
	{
		return false;
	}
#ifdef WIN32
	return (WAIT_OBJECT_0 == WaitForSingleObject(m_semaphore,INFINITE));
#elif defined __IOS__
	return (KERN_SUCCESS == semaphore_wait(m_Semaphore));
#else
	return (0 == sem_wait(&m_Semaphore));
#endif
}
bool Semaphore::Wait(int nTimeOut/*nMilliseconds*/)
{
	if ( m_semaphore == 0 )
	{
		return false;
	}
#ifdef WIN32
	return (WAIT_OBJECT_0 == WaitForSingleObject(m_semaphore,nTimeOut));
#else
	struct timespec ts;
#ifndef __IOS__
	clock_gettime(CLOCK_REALTIME, &ts);
	int timeoutSec = nTimeout/1000;
	int timeoutMilliSec = nTimeout%1000;
	ts.tv_sec += timeoutSec;
	int tsMilliSec = ts.tv_nsec/1000/1000; //nanosecond to microsecond to millisecond  
	if ( tsMilliSec + timeoutMilliSec < 1000 )
	{
		ts.tv_nsec = ts.tv_nsec + timeoutMilliSec*1000*1000;
	}
	else
	{
		ts.tv_sec += 1;
		ts.tv_nsec =  (tsMilliSec + timeoutMilliSec -1000)*1000*1000;		
	}
#else
	ts.tv_sec = nTimeout / 1000;
	ts.tv_nsec = (nTimeout % 1000) * 1000000;
#endif
	return ( 0 == sem_timedwait(&m_Semaphore, &ts));
#endif

}


}

#endif
