/*
	thread.h  
*/

#ifndef __NICK_THREAD__ 
#define __NICK_THREAD__ 

#if _MSC_VER > 1000
#pragma once
#endif 
#include "safequeue.h"

namespace COMMON
{

class Thread;
struct ThreadParam
{
	Thread * pThread;
	void * pParams;
};
class Thread
{
public:
	Thread();
	virtual ~Thread();

	virtual bool Start(void * pParams);
	virtual bool Stop();
	virtual bool SynStart(void * pParams, int nTimeOut); //miliseconds
	virtual bool SynStop(int nTimeOut); //miliseconds

	static unsigned long __stdcall ThreadProc(void *pParams);
	static void Msleep(int nMiliSeconds);

	int GetThreadID(){ return m_nThreadId ;}
	bool IsExit(){ return m_bRunning != true ; }
protected:
	virtual void Run(void * pParams) = 0;
protected:
	bool m_bRunning;
	int m_nThreadId;
	ThreadParam m_sThreadParam ;
	SafeQueue<int> m_start_lock_queue;
	SafeQueue<int> m_stop_lock_queue;

	Mutex m_mutex;
};
Thread::Thread()
{
	m_bRunning = false;
}
Thread::~Thread()
{

}
bool Thread::Stop()
{
	m_bRunning = false;
	return true;
}
bool Thread::SynStart(void * pParams, int nTimeOut) //miliseconds
{
	SingleMutex sMutex(&m_mutex);
	m_sThreadParam.pParams = pParams;
	m_sThreadParam.pThread = this;

	m_bRunning = false;
	m_start_lock_queue.Clear();
	bool bRet = Start(&m_sThreadParam);
	if ( bRet == true )
	{
		int nValue = 0;
		bRet = m_start_lock_queue.Pop(nValue,nTimeOut);
	}

	return bRet ;

}
bool Thread::SynStop(int nTimeOut) //miliseconds
{
	SingleMutex sMutex(&m_mutex);
	m_stop_lock_queue.Clear();
	m_bRunning = false;
	int nValue = 0;
	m_stop_lock_queue.Pop(nValue,nTimeOut);

	return true ;
}
unsigned long __stdcall Thread::ThreadProc(void * pParams)
{
	ThreadParam * TParams = (ThreadParam*)pParams;
	Thread * pThread = TParams->pThread;
	pThread->m_bRunning = true;
	pThread->m_start_lock_queue.Push(1);
	pThread->Run(TParams->pParams);
	pThread->m_bRunning = false;
	pThread->m_stop_lock_queue.Push(1);
	return  0;
}
#ifdef WIN32
bool Thread::Start(void * pParams)
{
	m_sThreadParam.pParams = pParams ;
	m_sThreadParam.pThread = this;
	DWORD nThreadID;
	HANDLE hThread = CreateThread(NULL,0,ThreadProc,&m_sThreadParam,0,&nThreadID);
	if ( hThread != NULL )
	{
		m_nThreadId = (int)hThread;
	}
	CloseHandle(hThread);
	return true ;
}
void Thread::Msleep(int nMiliSeconds)
{
	Sleep(nMiliSeconds);
}
#else
bool Thread::Start(void * pParams)
{
	pthread_attr_t attr;
	pthread_attr_init(&attr);
#ifdef _AIX
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	pthread_attr_setstacksize(&attr, 10*1024);	//the default stack size is only 96KB!
#endif
	m_sThreadParam.pBaseThread = this;
	m_sThreadParam.lpParam = lpParam;
	pthread_t ThreadID;
	pthread_create(&ThreadID, &attr,
		ThreadFunc, &m_sThreadParam );
	pthread_detach(ThreadID);

	pthread_attr_destroy(&attr);

	m_ThreadId = (int)ThreadID;
	return true;
}
void Thread::Msleep(int milliseconds)
{
#ifndef _AIX
	struct timeval tm;    
	tm.tv_sec = 0; 
	tm.tv_usec = milliseconds * 1000; 
	select(0,NULL,NULL,NULL,&tm);
#else
	usleep(milliseconds*1000);
#endif
}

#endif


}

#endif 