// LoginDlg.cpp: 实现文件
//

#include "stdafx.h"
#include <string>
#include "ADCT.h"
#include "LoginDlg.h"
#include "afxdialogex.h"
#include "OrderDlg.h"
#include "SQLManager.h"
#include "PublicFunction.h"


// LoginDlg 对话框

IMPLEMENT_DYNAMIC(LoginDlg, CDialogEx)

LoginDlg::LoginDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_Login_Dialog, pParent)
{

}

LoginDlg::~LoginDlg()
{
}

void LoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(LoginDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &LoginDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// LoginDlg 消息处理程序


void LoginDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString UserName, Pwd;
	GetDlgItemText(IDC_UserName_TB, UserName);
	GetDlgItemText(IDC_Pwd_TB, Pwd);
	_RecordsetPtr getRecord;
	SQLManager sqlManager;
	CString CheckStr="";
	if (!SQLIsUsing)
	{
		SQLIsUsing = true;
		if (sqlManager.ConnSQL())
		{
			try
			{
				getRecord = sqlManager.SQLCommand("SELECT *"
					" FROM [GPSTest].[dbo].[LUserAccount]"
					" WHERE Name = '" + UserName + "' AND Password='" + Pwd + "'"
				);
				while (!getRecord->adoEOF) {
					CheckStr = getRecord->GetCollect("Name");
					getRecord->MoveNext();
				}
			}
			catch (const std::exception&)
			{
				AfxMessageBox("数据库连接失败");
			}
			
		}
		else
		{
			AfxMessageBox("数据库连接失败");
		}
		sqlManager.CloseAll();
		SQLIsUsing = false;
	}
	if (CheckStr=="")
	{
		AfxMessageBox("登录失败");
		return;
	}
	/*if (UserName!="1")
	{
		if (Pwd != "1")
		{
			return;
		}
		return;
	}*/
	OrderDlg *Dlg = new OrderDlg();
	//MainForm->Create(IDD_ADCT_DIALOG);
	//MainForm->ShowWindow(true);
	this->EndDialog(0);
	
	//UpdateData(TRUE);
	CDialogEx::OnOK();
	Dlg->DoModal();
}


BOOL LoginDlg::PreTranslateMessage(MSG* pMsg)
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

void LoginDlg::GetSQLSetting()
{
	CPublicFunction pf;
	CString sCurPath = pf.GetCurDir();
	CFileStatus Status;
	CString m_sIniFile = sCurPath + "ADCT.ini";
	if (!CFile::GetStatus(m_sIniFile, Status))    
	{
		WritePrivateProfileString("SQLSetting", "SQLAddress", "10.10.11.90,1456", m_sIniFile);
		WritePrivateProfileString("SQLSetting", "User", "sa", m_sIniFile);
		WritePrivateProfileString("SQLSetting", "Password", "jiminewpower", m_sIniFile);
		WritePrivateProfileString("SQLSetting", "Database", "GPSTest", m_sIniFile);
		WritePrivateProfileString("SQLSetting", "FTPAddress", "120.77.221.153", m_sIniFile);
		WritePrivateProfileString("SQLSetting", "FTPPort", "21", m_sIniFile);
		WritePrivateProfileString("SQLSetting", "FTPUserName", "jimiftp", m_sIniFile);
		WritePrivateProfileString("SQLSetting", "FTPPWD", "jimiftp", m_sIniFile);
		sqlSettingInfo.Database = "GPSTest";
		sqlSettingInfo.SQLAddress = "10.10.11.90,1456";
		sqlSettingInfo.UserName = "sa";
		sqlSettingInfo.Password = "jiminewpower";
		sqlSettingInfo.FTPAddress = "120.77.221.153";
		sqlSettingInfo.FTPPort = "21";
		sqlSettingInfo.FTPUserName = "jimiftp";
		sqlSettingInfo.FTPPWD = "jimiftp";
	}
	else
	{
		CString strTemp;

		GetPrivateProfileString("SQLSetting", "SQLAddress", NULL, strTemp.GetBuffer(128), 128, m_sIniFile);
		sqlSettingInfo.SQLAddress = strTemp;
		GetPrivateProfileString("SQLSetting", "User", NULL, strTemp.GetBuffer(128), 128, m_sIniFile);
		sqlSettingInfo.UserName = strTemp;
		GetPrivateProfileString("SQLSetting", "Password", NULL, strTemp.GetBuffer(128), 128, m_sIniFile);
		sqlSettingInfo.Password = strTemp;
		GetPrivateProfileString("SQLSetting", "Database", NULL, strTemp.GetBuffer(128), 128, m_sIniFile);
		sqlSettingInfo.Database = strTemp;
		GetPrivateProfileString("SQLSetting", "FTPAddress", NULL, strTemp.GetBuffer(128), 128, m_sIniFile);
		sqlSettingInfo.FTPAddress = strTemp;
		GetPrivateProfileString("SQLSetting", "FTPPort", NULL, strTemp.GetBuffer(128), 128, m_sIniFile);
		sqlSettingInfo.FTPPort = strTemp;
		GetPrivateProfileString("SQLSetting", "FTPUserName", NULL, strTemp.GetBuffer(128), 128, m_sIniFile);
		sqlSettingInfo.FTPUserName = strTemp;
		GetPrivateProfileString("SQLSetting", "FTPPWD", NULL, strTemp.GetBuffer(128), 128, m_sIniFile);
		sqlSettingInfo.FTPPWD = strTemp;
		strTemp.ReleaseBuffer(128);
	}
}

BOOL LoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	GetSQLSetting();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
