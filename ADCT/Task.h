// Task.h: interface for the CTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASK_H__C95E4014_8C9E_4AD9_8AF7_62D73513DB33__INCLUDED_)
#define AFX_TASK_H__C95E4014_8C9E_4AD9_8AF7_62D73513DB33__INCLUDED_
#include "stdafx.h"
#if _MSC_VER > 1000
#pragma once

#endif // _MSC_VER > 1000

#define PORT_AUTO_THREAD     23001
#define PORT_SEND_THREAD     23002
#define PORT_LISTEN_THREAD   23003
#define POWER_RECV_LISTEN_THREAD  23004
//#define PORT_CONTROL_THREAD   23005
/** 线程退出标志变量 */
extern bool s_bExit;
extern bool m_SendThread;
extern bool m_ListenThread;


class CTask  //任务基类
{
private:

	//PortMessage m_pData;
	int threadCommand;

public:
	CTask();
	virtual ~CTask();
	//void SetData(PortMessage pData){m_pData=pData;};
	//PortMessage GetData(){return m_pData;};
	virtual void Run();
	void SetCommand(int setCommand) { threadCommand = setCommand; };
	int GetCommand() { return threadCommand; };
protected:

	
};

#endif // !defined(AFX_TASK_H__C95E4014_8C9E_4AD9_8AF7_62D73513DB33__INCLUDED_)
