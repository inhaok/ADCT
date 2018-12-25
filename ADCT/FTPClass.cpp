#pragma once
#include "stdafx.h"
#include "FTPClass.h"
#include "afxdialogex.h"
#include <string>
#include <stdio.h>
#include <afxinet.h>  //FTP���ͷ�ļ���
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
		//psMyCFtpConnection = psMyCInternetSession->GetFtpConnection("ftp.sdedu.net"); //��¼������
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
		AfxMessageBox("�������Ӵ���");
		return false;
	}
}
bool FTPClass::FindFile(CString FullName)
{
	pFtpFFind = new CFtpFileFind(psMyCFtpConnection);
	BOOL IsFindFile = pFtpFFind->FindFile(FullName);//��FTP���ҵ��ļ�
	if (IsFindFile)
	{
		m_downloadInfo.FileFullName = FullName;
	}
	else
	{
		AfxMessageBox("��FTP�������У�û���ҵ��ļ�");
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
		AfxMessageBox("�ļ����س���"+ ErrorCode);
		mainform.GetDownloadInfo(Download_Error);
	}*/
	return 0;
}
UINT DownloadStatusThreadFun(LPVOID pParam)
{
	DownloadFilePath* pathInfo = (DownloadFilePath*)pParam;
	CFileStatus status;
	
	m_downloadInfo.LocalFileSize = 0;//�Ƚ��ļ���С��ֵΪ0,ѭ����ֵ
	while (m_downloadInfo.LocalFileSize <=  pathInfo->ServerFileSize)
	{
		CFile::GetStatus(pathInfo->LocalPath, status);
		m_downloadInfo.LocalFileSize = status.m_size;
		if (m_downloadInfo.LocalFileSize == pathInfo->ServerFileSize)
		{
			m_DownloadExit = true;
			//AfxMessageBox("�������");
			HWND OrderHwnd = ::FindWindow("ADCT", "����ѡ��");
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
	MyThread = AfxBeginThread(DownloadThreadFun, &m_downloadInfo);//�����߳�
	MyThread2 = AfxBeginThread(DownloadStatusThreadFun, &m_downloadInfo);//�����߳�
}

void FTPClass::DownloadStop()
{
	m_DownloadExit = true;
}
