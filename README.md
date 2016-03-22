# threadpool
this is a threadpool for sever , it can run at windows/linux/ios/android
threadpool is writed by c++ 

next i will show the code of threadpool for using

  Process<T> m_Process;         
  //定义一个处理器
  
  void Init(int nTask,int nThread);
	//init：初始化任务和线程，以及准备一些变量的初始化
	//ntask：初始化一个任务数量，这个数量为了防止每次来了新任务都进行push和pop操作，所以在初始化的时候，传进去一个初始值，如果任务量比这个初始值还要大的话，进在进行添加
	//nThread：初始化一个线程池的数量，如果定义1000个线程，那么这个线程池中也就有1000的线程，每次线程有忙闲状态，如果任务量没有当前可处理的现成，那么就等待，等有空闲的线程，就从线程池中取出可用的线程进行处理
	
	void Uninit();
	//Uninit：反初始化任务和线程
	
	void Start(void (__stdcall*WorkProc)(int nThreadID, void *pTask, void* pUser),void * pParams);
	//start：开启整个线程池
	//void (__stdcall*)(int nThreadID, void *pTask, void* pUser)：回调函数，这个回调函数用来处理自己的业务逻辑，nthreadID是线程当前的number号码，pTask：是自己定义的任务指针，pUser：用户自定义的参数。
	//pParams：用户自定义参数
	
	void Stop();
	//start：关闭整个线程池
	
	int PushTask(T * task);
	//PushTask：往任务队列中添加任务
	//task：任务指针
	
	void FreeTask(T * pTask);
	//FreeTask：释放task指针，由于整个线程池中的任务有线程池中自己创建的，也有外部后来创建的，所以每个任务处理完，都需要释放该任务，如果任务是线程池中的任务，那么内部初始化创建的话，则直接添加到空闲任务队列中，如果是外部创建的话，则delete掉。
	//task：任务指针
	
	T * GetTask();
	//GetTask：从线程池的任务队列中去取空闲的任务，如果此时没有空闲的任务，则返回NULL，有的话，就返回空闲的任务指针。

	
we should show the code to how to use,such as:

Process<int> m_Processm_Processm_Process;

void __stdcall DoSomething(int nThreadID, void *pTask, void* pUser)
{
    //dosomething
	  m_Process.FreeTask((int *)pTask);
}
int _tmain(int argc, _TCHAR* argv[])
{
    m_Process.Init(10000,1000);
	  m_Process.Start(DoSomething,NULL);
  	for ( int i = 0 ; i < 1000000 ; i++ )
  	{
  		int * pTask = m_Process.GetTask();
  		if ( pTask == NULL )
  		{
  			pTask = new int;
  			*pTask = i;
  			m_Process.PushTask(pTask);
  		}
  		else
  		{
  			*pTask = i;
  			m_Process.PushTask(pTask);
  		}
	  }
}
