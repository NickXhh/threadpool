/*
	process.h  
*/

#ifndef __NICK_PROCESS__ 
#define __NICK_PROCESS__ 

#if _MSC_VER > 1000
#pragma once
#endif 


#include "mutex.h"
#include "taskthread.h"
#include <list>
#include <hash_set>

#define  TASK_FULL		1
#define  TASK_NORUN		2
#define  TASK_SUCESS	0



typedef void (__stdcall* WORKINGRPROC)(int nThreadID, void *pTask, void* pUser);


template<class T>
class Process
{
	typedef std::list<T*> task_list;
	typedef TaskThread<Process> task_thread;
	typedef std::list<task_thread *> task_thread_list;
	
public:
	Process()
	{

	}
	~Process()
	{

	}
	void Init(int nTask,int nThread)
	{
		for ( int i = 0 ; i < nTask ; i++ )
		{
			T * pTask = new T;
			m_sTasks.insert(pTask);
			m_listfreeTasks.push_back(pTask);
		}

		m_listwaitTasks.clear();

		for ( int i = 0 ; i < nThread ; i ++ )
		{
			task_thread * pThread = new task_thread(i,this);
			m_listThreads.push_back(pThread);
			m_listwaitThreads.push_back(pThread);
		}

		m_nTasks = nTask;
		m_nThreads = nThread ;
	}
	void ClearUp()
	{
		Stop();

		for ( task_thread_list::iterator it = mThreads.begin();it != end() ; it++ )
		{
			task_thread * pThread = it;
			delete task_thread;
			pThread = NULL;
		}
		m_listThreads.clear();

		for ( task_list::iterator it = m_sTasks.begin();it != end() ; it++ )
		{
			T * pTask = it;
			delete pTask;
			pTask = NULL;
		}
		m_sTasks.clear();
		m_listfreeTasks.clear();
		m_listwaitTasks.clear();
		m_listwaitThreads.clear();

	}
	void Start(WORKINGRPROC pProc,void * pParams)
	{
		m_funcWorking = pProc;
		m_pUserParams = pParams;
		task_thread_list::iterator it = m_listwaitThreads.begin();
		for (;it != m_listwaitThreads.end() ; it++ )
		{
			task_thread * pThread = *it;
			pThread->Start();
		}
	}
	void Stop()
	{
		for ( task_thread_list::iterator it = m_listwaitThreads.begin();it != m_listwaitThreads.end() ; it++ )
		{
			it->Stop();
		}

		m_listfreeTasks.clear();
		for ( task_list::iterator it = m_sTasks.begin();it != m_sTasks.end() ; it++ )
		{
			T* task = it;
			m_listfreeTasks.push_back(task);
		}
		m_listwaitTasks.clear();

	}
	T * GetTask()
	{
		SingleMutex sMutex(&m_mutex_free);
		if ( m_listfreeTasks.size() == 0 )
		{
			return NULL; 
		}
		T * pTask = m_listfreeTasks.front();
		m_listfreeTasks.pop_front();
		return pTask;
	}
	void FreeTask(T * pTask)
	{
		if( m_sTasks.find(pTask) == m_sTasks.end())
		{
			delete pTask;
			pTask = NULL ;
			return ;
		}
			
		SingleMutex sMutex(&m_mutex_free);
		m_listfreeTasks.push_back(pTask);

	}
	int PushTask(T * task)
	{
		T * pTask = NULL;
		if( m_sTasks.find(task) == m_sTasks.end())
		{
			SingleMutex sMutex(&m_mutex_free);
			if ( m_listfreeTasks.size() == 0 )
			{
				m_sTasks.insert(pTask);
				m_nTasks ++ ;
				pTask = task;
			}
			else
			{
				pTask = m_listfreeTasks.front();
				m_listfreeTasks.pop_front();
				memcpy(pTask,task,sizeof(T));
			}

		}
		else
		{
			pTask = task;
		}
		{
			SingleMutex sMutex(&m_mutex_wait);
			m_listwaitTasks.push_back(pTask);
			NotifyNextThread();
		}
		return TASK_SUCESS;
	}
	void Working(task_thread * pThread)
	{
		T * pTask = NULL;
		{
			SingleMutex sMutex(&m_mutex_wait);
			if ( m_listwaitTasks.size() != 0 )
			{
				pTask = m_listwaitTasks.front();
				m_listwaitTasks.pop_front();
				if ( m_listwaitTasks.size() != 0 )
				{
					NotifyNextThread();
				}
			}
		}
		
		if ( pTask != NULL )
		{
			if ( m_funcWorking != NULL )
			{
				m_funcWorking(pThread->GetThreadNumber(),pTask,m_pUserParams);
			}
			return ;
		}
		JoinWaitThread(pThread);
		pThread->m_Event.Wait();
	}
private:
	void NotifyNextThread()
	{
		if ( m_listwaitThreads.size() == 0)
		{
			return ;
		}
		else
		{
			SingleMutex sMutex(&m_mutex_thread);
			task_thread * pThread = m_listwaitThreads.front();
			m_listwaitThreads.pop_front();
			pThread->m_Event.Set();
		}
	}
	void JoinWaitThread(task_thread * pThread)
	{
		SingleMutex sMutex(&m_mutex_thread);
		pThread->m_Event.ReSet();
		m_listwaitThreads.push_back(pThread);
	}
private:
	COMMON::Mutex m_mutex_thread;
	COMMON::Mutex m_mutex_wait;
	COMMON::Mutex m_mutex_free;

	task_list m_listfreeTasks;
	task_list m_listwaitTasks;
	stdext::hash_set<T*> m_sTasks;

	task_thread_list m_listThreads;
	task_thread_list m_listwaitThreads;

	int m_nTasks;
	int m_nThreads;

	void * m_pUserParams;
	WORKINGRPROC m_funcWorking;


};


#endif