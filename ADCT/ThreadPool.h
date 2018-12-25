// ThreadPool.h: interface for the CThreadPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREADPOOL_H__0E035F6E_1450_410C_96E2_F4640C404D7D__INCLUDED_)
#define AFX_THREADPOOL_H__0E035F6E_1450_410C_96E2_F4640C404D7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>

#include "Task.h"
using namespace std;
class CThreadPool;
typedef struct ThreadItem
{
	HANDLE hThreadHandle;       //�߳̾��
	DWORD  dwThreadID;          //�߳�ID
	BOOL   bThreadFlag;         //�߳����б�ʶ
	CThreadPool* pThreadPool;   //�����ĸ��̳߳�
	ThreadItem()
	{
		hThreadHandle=NULL;
		dwThreadID=0;
		bThreadFlag=FALSE;
		pThreadPool=NULL;
	}
}ThreadItem;

class CThreadPool  
{
public:
	CThreadPool();
	CThreadPool(int nThreadCount);
	virtual ~CThreadPool();
	static DWORD WINAPI ThreadFunc(LPVOID lpParameter = NULL);
public:
	void AddTask(CTask* pTask);
	list<ThreadItem*>* GetThreadItemList(){return m_pThreadItemList;};
	list<CTask*>*      GetTaskList(){return m_pTaskList;};
	void               StopAllThread();
	void               AdjustSize(int nThreadCount);  //��̬�����̳߳ع�ģ
protected:
	//static 
private:
	int m_nThreadCount;    //�̳߳����̵߳ĸ���
	list<ThreadItem*>*  m_pThreadItemList;
	list<CTask*>*       m_pTaskList;
	CRITICAL_SECTION    m_csThreadQueue;
	CRITICAL_SECTION    m_csTaskQueue;
};

#endif // !defined(AFX_THREADPOOL_H__0E035F6E_1450_410C_96E2_F4640C404D7D__INCLUDED_)
