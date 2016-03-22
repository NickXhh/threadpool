
/*
	mutex.h  
*/

#ifndef __NICK_MUTEX__ 
#define __NICK_MUTEX__ 

#if _MSC_VER > 1000
#pragma once
#endif 


namespace COMMON
{

class Mutex
{
public:
	Mutex();
	virtual ~Mutex();
	void Lock();
	void UnLock();
	bool TryLock();

private:
#ifdef WIN32
	CRITICAL_SECTION m_mutex;
#endif
};
Mutex::Mutex()
{
#ifdef WIN32
	 InitializeCriticalSection(&m_mutex);
#else

	pthread_mutexattr_t l_attributes;
	::pthread_mutexattr_init(&l_attributes);
	::pthread_mutexattr_settype(&l_attributes, PTHREAD_MUTEX_RECURSIVE);
	::pthread_mutex_init(&m_mutex, &l_attributes);
	::pthread_mutexattr_destroy(&l_attributes);
#endif
}
Mutex::~Mutex()
{
#ifdef WIN32
	DeleteCriticalSection(&m_mutex);
#else
	pthread_mutex_destroy(&m_mutex);
#endif
}
void Mutex::Lock()
{
#ifdef WIN32
	EnterCriticalSection(&m_mutex);
#else
	pthread_mutex_lock(&m_mutex);
#endif
}
void Mutex::UnLock()
{
#ifdef WIN32
	LeaveCriticalSection(&m_mutex);
#else
	pthread_mutex_unlock(&m_mutex);
#endif
}
bool Mutex::TryLock()
{
#ifdef WIN32
	return ( true == TryEnterCriticalSection(&m_mutex));
#else
	return ( 0 == pthread_mutex_trylock(&m_mutex));
#endif
}

class SingleMutex
{
public:
	SingleMutex(Mutex * pMutex);
	virtual ~SingleMutex();

private:
	Mutex * m_mutex;

};
SingleMutex::SingleMutex(Mutex * pMutex):m_mutex(pMutex)
{
	m_mutex->Lock();
}
SingleMutex::~SingleMutex()
{
	m_mutex->UnLock();
}

}



#endif