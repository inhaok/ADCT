#pragma once
#include "stdafx.h"
#include "FTPClass.h"
#include "afxdialogex.h"
#include <string>
#include <stdio.h>
#include <afxinet.h>  //FTP类的头文件。
#include <fstream>
#include <stdlib.h>

using namespace std;

CFtpConnection *psMyCFtpConnection = NULL;
CInternetSession *psMyCInternetSession = NULL;
CFtpFileFind *pFtpFFind = NULL;
DownloadFilePath m_downloadInfo;
bool m_DownloadExit=true;


FTPClass::FTPClass()
{
}
bool FTPClass::FTPConnect(CString IPAddress,int Port,CString username,CString password) 
{
	psMyCInternetSession = new CInternetSession(
		"GATEWAY FTP", 1, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	try
	{
		psMyCFtpConnection = psMyCInternetSession->
			GetFtpConnection(IPAddress, username, password, Port);
		//psMyCFtpConnection = psMyCInternetSession->GetFtpConnection("ftp.sdedu.net"); //登录服务器
		return true;
	}
	catch (CInternetException*e)
	{
		delete   psMyCInternetSession;
		delete   psMyCFtpConnection;
		psMyCFtpConnection = NULL;
		return false;
	}

	if (psMyCFtpConnection == NULL)
	{
		delete   psMyCInternetSession;
		delete   psMyCFtpConnection;
		psMyCFtpConnection = NULL;
		AfxMessageBox("网络连接错误");
		return false;
	}
}
bool FTPClass::FindFile(CString FullName)
{
	pFtpFFind = new CFtpFileFind(psMyCFtpConnection);
	BOOL IsFindFile = pFtpFFind->FindFile(FullName);//在FTP上找到文件
	if (IsFindFile)
	{
		m_downloadInfo.FileFullName = FullName;
	}
	else
	{
		AfxMessageBox("在FTP服务器中，没有找到文件");
	}
	return IsFindFile;
}
UINT DownloadThreadFun(LPVOID pParam)
{
	DownloadFilePath* pathInfo = (DownloadFilePath*)pParam;
	int getError = psMyCFtpConnection->GetFile(pathInfo->FileFullName, pathInfo->LocalPath);
	/*if (getError==0)
	{
		CString ErrorCode;
		ErrorCode.Format("%d",GetLastError());
		AfxMessageBox("文件下载出错："+ ErrorCode);
		mainform.GetDownloadInfo(Download_Error);
	}*/
	return 0;
}
UINT DownloadStatusThreadFun(LPVOID pParam)
{
	DownloadFilePath* pathInfo = (DownloadFilePath*)pParam;
	CFileStatus status;
	
	m_downloadInfo.LocalFileSize = 0;//先将文件大小赋值为0,循环赋值
	while (m_downloadInfo.LocalFileSize <=  pathInfo->ServerFileSize)
	{
		CFile::GetStatus(pathInfo->LocalPath, status);
		m_downloadInfo.LocalFileSize = status.m_size;
		if (m_downloadInfo.LocalFileSize == pathInfo->ServerFileSize)
		{
			m_DownloadExit = true;
			//AfxMessageBox("下载完成");
			HWND OrderHwnd = ::FindWindow("ADCT", "工单选择");
			::SendMessage(OrderHwnd, WM_OrderDlgMessage, MessageDownloadSuccess,0);
			break;
		}
		/*if (m_DownloadExit)
		{
			break;
		}
*/
	}
	

	return 0;
}
void FTPClass::DownloadStart()
{
	m_DownloadExit = false;
	CWinThread *MyThread, *MyThread2;
	MyThread = AfxBeginThread(DownloadThreadFun, &m_downloadInfo);//下载线程
	MyThread2 = AfxBeginThread(DownloadStatusThreadFun, &m_downloadInfo);//进度线程
}

void FTPClass::DownloadStop()
{
	m_DownloadExit = true;
}
