#pragma once
#include <process.h>
#include <Windows.h>

typedef  struct 
{
	CString FileFullName;
	CString LocalPath;
	int ServerFileSize;
	int LocalFileSize;
	
}DownloadFilePath;
extern DownloadFilePath m_downloadInfo;
extern bool m_DownloadExit;


class FTPClass
{


public:
	FTPClass();
	bool FTPConnect(CString IPAddress, int Port, CString username, CString password);
	bool FindFile(CString FullName);
	void DownloadStart();

	void DownloadStop();
	

};

