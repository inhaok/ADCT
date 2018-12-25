// ThreadPool.cpp: implementation of the CThreadPool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "ThreadPoolTest.h"
#include "ThreadPool.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThreadPool::CThreadPool()
{
	m_nThreadCount=6; //默认创建
	InitializeCriticalSection(&m_csThreadQueue);
	InitializeCriticalSection(&m_csTaskQueue);
	m_pThreadItemList=new list<ThreadItem*>;
	m_pTaskList=new list<CTask*>;
	EnterCriticalSection(&m_csThreadQueue);
	for (int i=0;i<m_nThreadCount;i++)
	{
		ThreadItem* pItem=new ThreadItem();
		if (pItem)
		{
			pItem->pThreadPool=this;
			pItem->bThreadFlag=TRUE;
			pItem->hThreadHandle=CreateThread(NULL, 0, ThreadFunc, (LPVOID)pItem, 0, &pItem->dwThreadID);
			TRACE("创建线程 threadid:%d\n",pItem->dwThreadID);
			m_pThreadItemList->push_back(pItem);
		}
	}
	LeaveCriticalSection(&m_csThreadQueue);
}

CThreadPool::CThreadPool(int nThreadCount )
{
	m_nThreadCount=nThreadCount; 
	InitializeCriticalSection(&m_csThreadQueue);
	InitializeCriticalSection(&m_csTaskQueue);
	m_pThreadItemList=new list<ThreadItem*>;
	m_pTaskList=new list<CTask*>;
	EnterCriticalSection(&m_csThreadQueue);
	for (int i=0;i<m_nThreadCount;i++)
	{
		ThreadItem* pItem=new ThreadItem();
		if (pItem)
		{
			pItem->pThreadPool=this;
			pItem->bThreadFlag=TRUE;
			pItem->hThreadHandle=CreateThread(NULL, 0, ThreadFunc, (LPVOID)pItem, 0, &pItem->dwThreadID);
			TRACE("创建线程 threadid:%d\n",pItem->dwThreadID);
			m_pThreadItemList->push_back(pItem);
		}
	}
	LeaveCriticalSection(&m_csThreadQueue);
}
CThreadPool::~CThreadPool()
{
	StopAllThread();
	DeleteCriticalSection(&m_csThreadQueue);
	DeleteCriticalSection(&m_csTaskQueue);
	delete m_pTaskList;
	delete m_pThreadItemList;
}

void CThreadPool::AddTask( CTask* pTask )
{
	EnterCriticalSection(&m_csTaskQueue);
	if (m_pTaskList)
	{
		m_pTaskList->push_back(pTask);
	}
	LeaveCriticalSection(&m_csTaskQueue);
}

DWORD WINAPI CThreadPool::ThreadFunc( LPVOID lpParameter /*= NULL*/ )
{
	ThreadItem* pItem=(ThreadItem*)lpParameter;
	if (pItem)
	{
		while (pItem->bThreadFlag)
		{			
			EnterCriticalSection(&pItem->pThreadPool->m_csTaskQueue);
			CTask* pTask=NULL;
			if (!pItem->pThreadPool->GetTaskList()->empty())
			{
				pTask=pItem->pThreadPool->GetTaskList()->front();
			}
			else
			{//任务队列为空
				LeaveCriticalSection(&pItem->pThreadPool->m_csTaskQueue);
				Sleep(1);
				continue;
			}
			if (pTask)
			{
				pItem->pThreadPool->GetTaskList()->pop_front();
				LeaveCriticalSection(&pItem->pThreadPool->m_csTaskQueue);
				//TRACE("线程id %d 处理 %s \n",pItem->dwThreadID,pTask->GetCommand());
				pTask->Run();
				delete pTask;
				continue;
			}
			LeaveCriticalSection(&pItem->pThreadPool->m_csTaskQueue);

			Sleep(5);
		}
	}
	return 0;
}

void CThreadPool::StopAllThread()
{
	EnterCriticalSection(&m_csThreadQueue);
	while (!m_pThreadItemList->empty())
	{		
		ThreadItem* pThreadItem=m_pThreadItemList->front();
		if (pThreadItem)
		{
			TRACE("结束线程 %d\n",pThreadItem->dwThreadID);
			pThreadItem->bThreadFlag=FALSE;
			WaitForSingleObject(pThreadItem->hThreadHandle,INFINITE);
			m_pThreadItemList->pop_front();
			delete(pThreadItem);			
		}		
	}
	LeaveCriticalSection(&m_csThreadQueue);
}

void CThreadPool::AdjustSize( int nThreadCount )
{
	if (nThreadCount==m_nThreadCount && nThreadCount<0)
	{
		return;
	}
	if (nThreadCount>m_nThreadCount)
	{//需要增加 nThreadCount-m_nThreadCount 个线程
		EnterCriticalSection(&m_csThreadQueue);
		for (int i=0;i<(nThreadCount-m_nThreadCount);i++)
		{
			ThreadItem* pItem=new ThreadItem();
			if (pItem)
			{
				pItem->pThreadPool=this;
				pItem->bThreadFlag=TRUE;
				pItem->hThreadHandle=CreateThread(NULL, 0, ThreadFunc, (LPVOID)pItem, 0, &pItem->dwThreadID);
				TRACE("创建线程 threadid:%d\n",pItem->dwThreadID);				
				m_pThreadItemList->push_back(pItem);
			}
		}

		TRACE("增加 %d 个线程\n",nThreadCount-m_nThreadCount);
		m_nThreadCount=nThreadCount;
		
		LeaveCriticalSection(&m_csThreadQueue);
	}
	if (nThreadCount<m_nThreadCount)
	{
		EnterCriticalSection(&m_csThreadQueue);
		for (int i=0;i<m_nThreadCount-nThreadCount;i++)
		{
			if (!m_pThreadItemList->empty())
			{
				ThreadItem* pItem=m_pThreadItemList->back();
				m_pThreadItemList->pop_back();
				pItem->bThreadFlag=FALSE;
				WaitForSingleObject(pItem->hThreadHandle,INFINITE);
				delete(pItem);
			}
		}
		TRACE("删除 %d 个线程",m_nThreadCount-nThreadCount);
		LeaveCriticalSection(&m_csThreadQueue);
	}
}