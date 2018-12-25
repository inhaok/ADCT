#include "stdafx.h"
#include "ADCTDlg.h"
#include "PowerManager.h"
#include "Task.h"
#include "PublicFunction.h"

HANDLE hCom;
CString GetPortRecv = "";
CStringArray	CommandList;
CStringArray	CommandListBackup;
CStringArray	CheckRecvList;
bool ReadOff = true;
bool ThreadIsCreate = false;
CThreadPool *m_lpThreadPool = NULL;


PowerManager::PowerManager()
{
	
}



PowerManager::~PowerManager()
{
}

void PowerManager::CreateThread()
{
	if (!ThreadIsCreate)
	{
		ThreadIsCreate = true;
		
		if (!m_lpThreadPool)
		{
			m_lpThreadPool = new CThreadPool();
		}
	}
	
}


void PowerManager::ClosePort()
{
	/** 如果有串口被打开，关闭它 */
	if (hCom != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hCom);
		hCom = INVALID_HANDLE_VALUE;
	}
}

bool PowerManager::openPort(UINT portNo)
{
	ClosePort();
	char szPort[50];
	if (portNo > 9)
	{
		sprintf_s(szPort, "\\\\.\\COM%d", portNo);
	}
	else
	{
		sprintf_s(szPort, "COM%d", portNo);
	}
	hCom = CreateFile(szPort,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (hCom == (HANDLE)-1)
	{
		AfxMessageBox("打开COM 失败!");
		return false;
	}

	SetupComm(hCom, 100, 100);
	COMMTIMEOUTS TimeOuts;
	//严重警告，注意超时带来的内存溢出
	//设定读超时
	TimeOuts.ReadIntervalTimeout = 100;
	TimeOuts.ReadTotalTimeoutMultiplier = 5;
	TimeOuts.ReadTotalTimeoutConstant = 5;
	//在读一次输入缓冲区的内容后读操作就立即返回,
	//而不管是否读入了要求的字符.
	//设定写超时
	TimeOuts.WriteTotalTimeoutMultiplier = 100;
	TimeOuts.WriteTotalTimeoutConstant = 100;
	SetCommTimeouts(hCom, &TimeOuts);//设置超时
	DCB dcb;
	GetCommState(hCom, &dcb);
	dcb.BaudRate = 9600;//波特率
	dcb.ByteSize = 8;//每个字节有8 位
	dcb.Parity = NOPARITY;//无奇偶校验位
	dcb.StopBits = ONESTOPBIT;//1个停止位
	SetCommState(hCom, &dcb);
	PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);
	return true;
}

void PowerManager::OpenThreadPoolTask(int Command)
{
	CTask* pTask = new CTask;
	//int pCommand = new char[20];
	//memset(pData, 0, 20);
	//sprintf(pData, "task %d", i);
	pTask->SetCommand(Command);
	m_lpThreadPool->AddTask(pTask);
}

void PowerManager::ListenThread()
{
	GetPortRecv = "";
	unsigned char strtemp[20];
	memset(strtemp, 0, sizeof(strtemp));
	CPublicFunction pf;
	CString str;
	DWORD readreal = 0;
	BOOL bReadStat;
	CString temp;
	//Sleep(200);
	do
	{
		str = "";
		Sleep(200);
		if (hCom== INVALID_HANDLE_VALUE)
		{
			return;
		}
		bReadStat = ReadFile(hCom, strtemp, 20, &readreal, 0);
		//str.ReleaseBuffer();
		/*if (str=="")
		{
			continue;
		}*/
		do
		{
			Sleep(50);
		} while (ReadOff==false&&!s_bExit == false);
		if (bReadStat)
		{
			//AfxMessageBox(str);
			/*GetPortRecv= pf.HexStringOperation(str);
			AfxMessageBox(GetPortRecv);
			str.ReleaseBuffer();*/
			
			/*for (int i = 0; i < sizeof(strtemp); i++)
			{
				GetPortRecv.Format(GetPortRecv +"%02X", strtemp);
			}
			AfxMessageBox(GetPortRecv);*/
			try
			{
				for (int i = 0; i < 8; i++)
				{
					temp.Format("%02X", strtemp[i]);
					GetPortRecv += temp;
					ReadOff = false;
				}
			}
			catch (const std::exception&)
			{

			}
			
			//PurgeComm(hCom,PURGE_RXCLEAR | PURGE_TXCLEAR);
		}

	} while (!s_bExit);
	//*strtemp=NULL;
	PurgeComm(hCom, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_TXABORT);
}

void PowerManager::SendThread()
{
	PurgeComm(hCom, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_TXABORT);
	char lpOutBuffer[100];
	string SendPortBuffer;
	DWORD dwBytesWrite;

	COMSTAT ComStat;

	DWORD dwErrorFlags;

	BOOL bWriteStat;

	ClearCommError(hCom, &dwErrorFlags, &ComStat);
	
	CPublicFunction pf;
	bool writeOk;
	do
	{
		for (int i = 0; i < 4; i++)
		{
			SendPortBuffer = CommandList.GetAt(i);
			dwBytesWrite = SendPortBuffer.length();
			if (SendPortBuffer == "")
			{
				continue;
			}
			BYTE * SendByte = pf.CStrToByte(SendPortBuffer.length(), SendPortBuffer.data());
			do {
				
				
				bWriteStat = WriteFile(hCom, SendByte, SendPortBuffer.length()/2, &dwBytesWrite, NULL);
				Sleep(atoi(SleepTimer));
			} while (0);
		}


	} while (!s_bExit);
}

void PowerManager::ReadPortRecvList()
{

	try
	{
		CString temp;
		s_bExit = false;
		int FindIndex;
		string getIn, getTemp, firstRead, doubleRead;
		do
		{
			Sleep(50);
			for (int i = 0; i < 4; i++)
			{
				if (GetPortRecv==""|| CommandList.GetAt(i)=="")
				{
					continue;
				}
				if (GetPortRecv.Find(CommandList.GetAt(i)) > -1)
				{
					CommandList.SetAt(i, "");
				}
			}
			GetPortRecv = "";
			ReadOff = true;

		} while (!s_bExit);
	}
	catch (const std::exception&e)
	{
		AfxMessageBox(e.what());
	}

}

void PowerManager::CommandListListen()
{
	CString GetTemp;
	do
	{


		for (int i = 0; i < 4; i++)
		{
			GetTemp = CommandListBackup.GetAt(i);
			if (GetTemp != "")
			{
				if (CommandList.GetAt(i) == "")
				{
					CommandList.SetAt(i, GetTemp);
					CommandListBackup.SetAt(i,"");
				}

			}

		}
	} while (!s_bExit);
}

void PowerManager::StartListen()
{

	CreateThread();
	OpenThreadPoolTask(PORT_AUTO_THREAD);
	OpenThreadPoolTask(PORT_LISTEN_THREAD);
	OpenThreadPoolTask(PORT_SEND_THREAD);
	OpenThreadPoolTask(POWER_RECV_LISTEN_THREAD);
}

char * PowerManager::HexDump(char *buf, int len) {
	int i, j, k;
	char binstr[80];

	for (i = 0; i < len; i++) {
		if (0 == (i % 16)) {
			sprintf_s(binstr, "%08x -", i + 000111);
			sprintf_s(binstr, "%s %02x", binstr, (unsigned char)buf[i]);
		}
		else if (15 == (i % 16)) {
			sprintf_s(binstr, "%s %02x", binstr, (unsigned char)buf[i]);
			sprintf_s(binstr, "%s  ", binstr);
			for (j = i - 15; j <= i; j++) {
				sprintf_s(binstr, "%s%c", binstr, ('!' < buf[j] && buf[j] <= '~') ? buf[j] : '.');
			}
			printf("%s\n", binstr);
		}
		else {
			sprintf_s(binstr, "%s %02x", binstr, (unsigned char)buf[i]);
		}
	}
	/*if (0 != (i % 16)) {
		k = 16 - (i % 16);
		for (j = 0; j < k; j++) {
			sprintf_s(binstr, "%s   ", binstr);
		}
		sprintf_s(binstr, "%s  ", binstr);
		k = 16 - k;
		for (j = i - k; j < i; j++) {
			sprintf_s(binstr, "%s%c", binstr, ('!' < buf[j] && buf[j] <= '~') ? buf[j] : '.');
		}
		
	}*/
	return binstr;
}