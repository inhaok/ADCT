#pragma once
#include "ThreadPool.h"

extern CString GetPortRecv;
extern bool	ReadOff;
extern bool ThreadIsCreate;
extern string RecvPortBuffer;
extern CStringArray	CommandList;
extern CStringArray	CommandListBackup;
extern CStringArray	CheckRecvList;
extern CThreadPool* m_lpThreadPool;
extern HANDLE hCom;


class PowerManager
{
public:
	PowerManager();
	~PowerManager();
	void CreateThread();
	void ClosePort();
	bool openPort(UINT portNo);
	void OpenThreadPoolTask(int Command);
	void ListenThread();
	void SendThread();
	void ReadPortRecvList();
	void CommandListListen();
	void StartListen();
	char * HexDump(char * buf, int len);
	
	
};

