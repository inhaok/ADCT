#include "stdafx.h"
#include "Task.h"
#include "PowerManager.h"
bool s_bExit = true;
bool m_SendThread=true;
bool m_ListenThread = true;
CTask::CTask()
{
}

CTask::~CTask()
{
}

void CTask::Run()
{
	PowerManager m_PowerManager;
	switch (threadCommand)
	{
	case PORT_AUTO_THREAD:
		m_PowerManager.ReadPortRecvList();
		break;
	case PORT_SEND_THREAD:
	{
		if (m_SendThread)
		{
			m_SendThread = false;
			m_PowerManager.SendThread();

		}
	}
	break;
	case PORT_LISTEN_THREAD:
	{
		if (m_ListenThread)
		{
			m_ListenThread = false;
			m_PowerManager.ListenThread();

		}
	}
	break;
	case POWER_RECV_LISTEN_THREAD:
	{
		m_PowerManager.CommandListListen();
	}
	break;
	default:
		break;
	}
}

