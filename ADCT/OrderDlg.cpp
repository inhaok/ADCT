// OrderDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "ADCT.h"
#include "afxdialogex.h"
#include "OrderDlg.h"
#include "SQLManager.h"
#include "ADCTDlg.h"
#include <string>
#include "FTPClass.h"
#include "PublicFunction.h"
#include "WinRARClass.h"

CString Version;
CString FTPPath;
int FileSize;
CStringArray AllOrderList;
CStringArray FuzzySearchOrderList;
CString SCFileFolderPath;
CStringArray FindFileArray;
CString m_MultiDownloadFilePath;
CString m_OrderName, m_VersionName;
// OrderDlg 对话框

using namespace std;
IMPLEMENT_DYNAMIC(OrderDlg, CDialogEx)

OrderDlg::OrderDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_Order_Dialog, pParent)
{

}

OrderDlg::~OrderDlg()
{
}

void OrderDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_OrderName_CCB, m_OrderName_CCB);
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(OrderDlg, CDialogEx)
	ON_BN_CLICKED(IDC_ReadOption_Btn, &OrderDlg::OnBnClickedReadoptionBtn)
	ON_CBN_EDITCHANGE(IDC_OrderName_CCB, &OrderDlg::OnCbnEditchangeOrdernameCcb)
	ON_MESSAGE(WM_OrderDlgMessage, &OrderDlg::OnOrderdlgmessage)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL OrderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_Download_PG = ((CProgressCtrl *)GetDlgItem(IDC_OrderDownload_PG));
	
	m_Download_PG->SetStep(10);
	m_Download_PG->SetPos(0);
	WinRARClass winrar;
	winrar.FindWinRARPath();
	SelectAllOrder();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


// OrderDlg 消息处理程序

void OrderDlg::SelectAllOrder()
{
	_RecordsetPtr getRecord;
	SQLManager sqlManager;
	if (!SQLIsUsing)
	{
		SQLIsUsing = true;
		if (sqlManager.ConnSQL())
		{
			try
			{
				AllOrderList.RemoveAll();
				getRecord = sqlManager.SQLCommand("SELECT ADCT_Order.OrderName FROM [GPSTest].[dbo].[ADCT_Order]");
				int OrderNum = 0;
				while (!getRecord->adoEOF) {
					AllOrderList.Add(getRecord->GetCollect("OrderName"));
					OrderNum++;
					getRecord->MoveNext();
					//AfxMessageBox(ReadID);
				}
			}
			catch (const std::exception&)
			{

			}
			sqlManager.CloseAll();

		}
		SQLIsUsing = false;
	
	
	}
}

bool OrderDlg::Download(CString IPAddress, CString Port, CString UserName, CString Password, CString fileName, CString serverFileName, int serverFileSize)
{
	FTPClass download;
	CPublicFunction *CPF = new CPublicFunction();
	string temp = CPF->GetCurDir();
	temp += "src\\";
	if (download.FTPConnect(IPAddress, atoi(Port), UserName, Password))
	{
		m_downloadInfo.LocalPath = temp.c_str() + fileName;
		m_downloadInfo.ServerFileSize = serverFileSize;
		if (download.FindFile(serverFileName))
		{
			//SetTimer(DownloadStatus_Timer, 100, NULL);

			download.DownloadStart();
			return true;
		}
		else
		{
			return false;
		}

		
	}
	else
	{
		AfxMessageBox("FTP连接失败");
		return false;
	}

}

void OrderDlg::OnBnClickedReadoptionBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_ReadOption_Btn)->EnableWindow(false);
	GetDlgItem(IDC_OrderName_CCB)->GetWindowText(m_OrderName);
	GetDlgItem(IDC_Version_TB)->GetWindowText(m_VersionName);
	_RecordsetPtr getRecord;
	SQLManager sqlManager;
	Version = "";
	CString AutoTestOrderCheck;
	if (!SQLIsUsing)
	{
		SQLIsUsing = true;
		if (sqlManager.ConnSQL())
		{
			try
			{
				getRecord = sqlManager.SQLCommand("SELECT *"
					" FROM [GPSTest].[dbo].[ADCT_Order], [GPSTest].[dbo].[ADCT_Version]"
					" WHERE ADCT_Order.VersionID = ADCT_Version.ID AND"
					" ADCT_Order.OrderName = '" + m_OrderName + "' AND VersionName='"+ m_VersionName +"'"
				);
				while (!getRecord->adoEOF) {
					Version = getRecord->GetCollect("VersionName");
					FTPPath = getRecord->GetCollect("Path");
					FileSize = getRecord->GetCollect("Size");
					getRecord->MoveNext();
					//AfxMessageBox(ReadID);
					
				}
				getRecord = sqlManager.SQLCommand("SELECT * FROM [NetMarkIMEI].[dbo].[TestSystemSettingFunc]"
					" WHERE"
					" SoftWare = '" + Version+m_OrderName + "'"
				);
				while (!getRecord->adoEOF) {
					AutoTestOrderCheck= getRecord->GetCollect("SoftWare");
					getRecord->MoveNext();
				}
				
			}
			catch (const std::exception&)
			{
				AfxMessageBox("订单查询时，数据库连接失败");
			}
			sqlManager.CloseAll();

		}
		SQLIsUsing = false;
		if (Version=="")
		{
			AfxMessageBox("无工单号或软件版本匹配");
			GetDlgItem(IDC_ReadOption_Btn)->EnableWindow(true);
			return;
		}
		if (AutoTestOrderCheck == "")
		{
			AfxMessageBox("自动测试订单单号或软件版本不匹配");
			GetDlgItem(IDC_ReadOption_Btn)->EnableWindow(true);
			return;
		}
		SendMessage(WM_OrderDlgMessage, MessageFindPathSuccess, 0);
	}
	if (Download(sqlSettingInfo.FTPAddress, sqlSettingInfo.FTPPort, sqlSettingInfo.FTPUserName, sqlSettingInfo.FTPPWD, FTPPath, FTPPath, FileSize))
	{
	}
	else
	{
		GetDlgItem(IDC_ReadOption_Btn)->EnableWindow(true);
	}
	m_Download_PG->SetRange32(0, FileSize);
	SetTimer(DownloadSizeCheck_Timer, 100, NULL);
}





void OrderDlg::OnCbnEditchangeOrdernameCcb()
{
	// TODO: 在此添加控件通知处理程序代码
	CString m_KeyWords;
	m_OrderName_CCB.GetWindowText(m_KeyWords);
	m_OrderName_CCB.ResetContent();
	FuzzySearch(AllOrderList, m_KeyWords, FuzzySearchOrderList);
	int OrderNum = FuzzySearchOrderList.GetSize();
	CRect rc;
	m_OrderName_CCB.GetDroppedControlRect(&rc);
	m_OrderName_CCB.GetParent()->ScreenToClient(&rc);
	rc.bottom += 10;
	m_OrderName_CCB.MoveWindow(&rc);
	for (int i = 0; i < OrderNum; i++)
	{
		m_OrderName_CCB.AddString(FuzzySearchOrderList.GetAt(i));
	}


	m_OrderName_CCB.ShowDropDown(true);
	m_OrderName_CCB.SetWindowText(m_KeyWords);
	m_OrderName_CCB.SetEditSel(m_KeyWords.GetLength() + 1, m_KeyWords.GetLength() + 1);
	SendMessage(WM_SETCURSOR, 0, 0);

	//ReleaseCapture();
}

void OrderDlg::FuzzySearch(CStringArray &FuzzySearchInList, CString KeyWords, CStringArray &FuzzySearchOutList)
{
	FuzzySearchOutList.RemoveAll();
	int MaxNum = FuzzySearchInList.GetSize();
	CString NowSearch;
	for (int i = 0; i < MaxNum; i++)
	{
		NowSearch = FuzzySearchInList.GetAt(i);
		if (NowSearch.Find(KeyWords) > -1)
		{
			FuzzySearchOutList.Add(NowSearch);
		}
	}
}

void OrderDlg::FindAllFiles(const char * lpPath)
{

	char szFind[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	strcpy_s(szFind, lpPath);
	strcat_s(szFind, "\\*.*");
	HANDLE hFind = ::FindFirstFile(szFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)
		return;

	do
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (strcmp(FindFileData.cFileName, ".") != 0 && strcmp(FindFileData.cFileName, "..") != 0)
			{
				//发现子目录，递归
				char szFile[MAX_PATH] = { 0 };
				strcpy_s(szFile, lpPath);
				strcat_s(szFile, "\\");
				strcat_s(szFile, FindFileData.cFileName);
				FindAllFiles(szFile);
			}
		}
		else
		{
			//找到文件
			char szFile[MAX_PATH] = { 0 };
			strcpy_s(szFile, lpPath);
			strcat_s(szFile, "\\");
			strcat_s(szFile, FindFileData.cFileName);
			FindFileArray.Add(szFile);
		}
	} while (::FindNextFile(hFind, &FindFileData));

	::FindClose(hFind);

}

afx_msg LRESULT OrderDlg::OnOrderdlgmessage(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case MessageDownloadSuccess:
	{
		//AfxMessageBox("开始解压");
		CPublicFunction pf;
		CString sCurPath = pf.GetCurDir();
		WinRARClass winrar;
		SCFileFolderPath = sCurPath + "src\\" + FTPPath;
		winrar.UnpackFile(SCFileFolderPath);

	}
	break;
	case MessageUnpackSuccess:
	{
		//AfxMessageBox("解压完成");
		CString Folder = SCFileFolderPath.Left(SCFileFolderPath.ReverseFind('.'));
		FindFileArray.RemoveAll();
		FindAllFiles(Folder);
		m_Download_PG->StepIt();
		SendMessage(WM_OrderDlgMessage, MessageFindFileSuccess, 0);
	}
	break;
	case MessageFindFileSuccess:
	{
		DisposeDownloadFile();
		m_Download_PG->StepIt();
		//AfxMessageBox("遍历完成");
		CADCTDlg MainForm;
		this->EndDialog(0);
		MainForm.OrderNameStatic = m_OrderName;
		MainForm.VersionStatic = m_VersionName;
		MainForm.MultiDownloadFilePath = m_MultiDownloadFilePath;
		MainForm.DoModal();
	}
	break;
	default:
		break;
	}
	return 0;
}

void OrderDlg::DisposeDownloadFile()
{
	for (int i = 0; i < FindFileArray.GetSize(); i++)
	{
		if (FindFileArray.GetAt(i).Find(".cfg") > 0)
		{
			CADCTDlg MainForm;
			m_MultiDownloadFilePath = FindFileArray.GetAt(i);
			m_MultiDownloadFilePath = m_MultiDownloadFilePath.Left(m_MultiDownloadFilePath.ReverseFind('\\'));
			//MainForm.ADCTCopyDataSend(1, "ALL", 1, 3, m_MultiDownloadFilePath.GetBuffer());
			//AfxMessageBox(MultiDownloadFilePath);
			break;
		}
	}
}

void OrderDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case DownloadSizeCheck_Timer:
	{
		m_Download_PG->SetPos(m_downloadInfo.LocalFileSize);
		if (m_downloadInfo.LocalFileSize == m_downloadInfo.ServerFileSize)
		{
			KillTimer(DownloadSizeCheck_Timer);
		}
	}
		break;
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}


BOOL OrderDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_ESCAPE: //Esc event
			return true;
		case VK_RETURN: //Enter event 
			return true;
		case VK_SPACE:
			return true;
		default:
			;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
