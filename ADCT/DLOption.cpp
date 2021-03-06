// DLOption.cpp: 实现文件
//
#pragma once

#include "stdafx.h"
#include "ADCT.h"
#include "DLOption.h"
#include "afxdialogex.h"
#include "ADCTDlg.h"
#include "PublicFunction.h"
#include<iostream>
#include <string>

using namespace std;
CString DAFileBackup = NULL;
CStringArray PortList;
string OptionsList[] = { "OpenDownloadAgentFile1","miOpenAuthFile","Show_scatter_and_load",
"OpenRootCertFile","OpenCertificateFile","miSetupDataSrc","miEnableFDLog","ShowFactoryDatabaseFile","miEnableNFICS1",
"miBBChipVersionCheck","m_miDnWithoutBkAndRsCalData","m_miDnWithBkAndRsCalData","m_miDnAndRsCalDataOnly","m_miDnAndUlUserData",
"m_miRsCalDataOnly","m_miUploadUserDataOnly","m_miCheckSDS","SetbeginCOMport1","USBDownloadWOBattery",
"mi_USBDownloadReadbackDataCard","mi_USBDownloadReadbackDualTalk","miEnableMMAA1","SendRootCertForcely",
"USBMetaModeAfterOneStationDownload1","EnableUSB11to20","m_DisableLongPressPWKEY","EnableBackupNvramPartion",
"miEnterMetaModeafterDLFMT1","miToolSetting","FlashSafeMode","miBromDllLog","miClearBromDllLog",
"miMETADllLog","miClearMETADllLog","miMDTLog","miClearMDTLog","ClearAllLogs","OpenDeviceManager1",
"SwitchToOperator1","ChangePassword1","SwitchToEngineer1","miAbout" };
string OptionsCheckList[] = { "miEnableFDLog","miEnableNFICS1"
,"miBBChipVersionCheck","m_miDnWithoutBkAndRsCalData","m_miDnWithBkAndRsCalData",
"m_miDnAndRsCalDataOnly","m_miDnAndUlUserData",
"m_miRsCalDataOnly","m_miUploadUserDataOnly","m_miCheckSDS","USBDownloadWOBattery","mi_USBDownloadReadbackDataCard",
"mi_USBDownloadReadbackDualTalk","miEnableMMAA1","SendRootCertForcely",
"USBMetaModeAfterOneStationDownload1","EnableUSB11to20","m_DisableLongPressPWKEY","EnableBackupNvramPartion",
"miEnterMetaModeafterDLFMT1","FlashSafeMode","miBromDllLog",
"miMETADllLog","miMDTLog" };


// DLOption 对话框

IMPLEMENT_DYNAMIC(DLOption, CDialogEx)

DLOption::DLOption(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLOption, pParent)
	, COMPortType(0)
	, DownloadFormatOperation(0)
	, FlashType(0)
{

}

DLOption::~DLOption()
{
}

void DLOption::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_stDAFileName_TB, m_DAFileName_TB);
	DDX_Control(pDX, IDC_Baud_CCB, m_BaudCCB);
	DDX_Control(pDX, IDC_DownloadType_CCB, m_DownloadTypeCCB);
	DDX_Radio(pDX, IDC_UART_RD, COMPortType);
	DDX_Radio(pDX, IDC_Auto_RD, DownloadFormatOperation);
	DDX_Radio(pDX, IDC_NAND_RD, FlashType);
	DDX_Control(pDX, IDC_PowerComSelect_CCB, m_PowerComCCB);
	CDialogEx::DoDataExchange(pDX);

}


BEGIN_MESSAGE_MAP(DLOption, CDialogEx)
	ON_COMMAND_RANGE(ID_FILE_OPENDOWNLOADAGENTFILE, ID_HELP_ABOUT, &DLOption::OnMenuClick)
	ON_BN_CLICKED(IDOK, &DLOption::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_DLOptionsMessage, &DLOption::OnDloptionsmessage)
	ON_BN_CLICKED(IDC_UART_RD, &DLOption::OnBnClickedUartRd)
	ON_BN_CLICKED(IDC_USB_RD, &DLOption::OnBnClickedUartRd)
	ON_BN_CLICKED(IDC_UARTUSB_RD, &DLOption::OnBnClickedUartRd)
	ON_BN_CLICKED(IDC_Auto_RD, &DLOption::OnBnClickedAutoRd)
	ON_BN_CLICKED(IDC_Manual_RD, &DLOption::OnBnClickedAutoRd)
	ON_BN_CLICKED(IDC_ResetToFactory_RD, &DLOption::OnBnClickedAutoRd)
	ON_BN_CLICKED(IDC_NAND_RD, &DLOption::OnBnClickedNandRd)
	ON_BN_CLICKED(IDC_NOR_RD, &DLOption::OnBnClickedNandRd)
END_MESSAGE_MAP()


// DLOption 消息处理程序


void DLOption::OnMenuClick(UINT nID)
{
	// TODO: 在此添加命令处理程序代码
	CADCTDlg MainForm;
	CString temp;
	int BtnID = nID - ID_FILE_OPENDOWNLOADAGENTFILE;
	temp.Format("%d", nID);
	//AfxMessageBox(temp);
	MainForm.ADCTCopyDataSend(1, "ALL", 1, 4, (char*)OptionsList[BtnID].c_str());
	/*for (int i = 0; i < OptionsCheckList->length(); i++)
	{
		if (OptionsList[BtnID].find(OptionsCheckList[i])>-1)
		{
			if (IsDlgButtonChecked(nID))
			{
				CheckDlgButton(nID, false);
			}
			else
			{
				CheckDlgButton(nID, true);
			}
			
		}
	}*/
	
}


BOOL DLOption::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CADCTDlg MainForm;
	DLOptionHWND = this->m_hWnd;
	GetDlgItem(IDC_stDAFileName_TB)->SetWindowText(DAFileBackup);
	GetDlgItem(IDC_stScatterFile_TB)->SetWindowText(DownloadFilePathBackup);
	CCBInit();
	m_BaudCCB.SelectString(-1, m_Baud);
	m_DownloadTypeCCB.SelectString(-1, m_DownloadType);
	SetPortCCB();
	GetDlgItem(IDC_RadioAddress_TB)->SetWindowText(m_RadioAddressText);
	GetDlgItem(IDC_PowerComSelect_CCB)->SetWindowText(m_PowerComText);
	GetDlgItem(IDC_SleepTimer_TB)->SetWindowText(m_SleepTimer);
	//DLTypeOption();
	UpdateData(false);
	for (int i = 0; i < 42; i++)
	{
		MainForm.ADCTCopyDataSend(1, "ALL", 0, 31, (char*)OptionsList[i].c_str());
		Sleep(20);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void DLOption::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_BaudCCB.GetWindowText(m_Baud);
	m_DownloadTypeCCB.GetWindowText(m_DownloadType);
	m_PowerComCCB.GetWindowText(m_PowerComText);
	GetDlgItem(IDC_RadioAddress_TB)->GetWindowText(m_RadioAddressText);
	GetDlgItem(IDC_SleepTimer_TB)->GetWindowText(m_SleepTimer);
	CDialogEx::OnOK();
}


void DLOption::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialogEx::OnClose();
}


afx_msg LRESULT DLOption::OnDloptionsmessage(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case 0:
	{
		CString *m_MenuCheckTemp = (CString *)lParam;
		for (int i = 0; i < 42; i++)
		{
			if (m_MenuCheckTemp->Find(OptionsList[i].c_str())>-1)
			{
				int FindID = i+ ID_FILE_OPENDOWNLOADAGENTFILE;
				CMenu * menu = GetMenu();
				menu->CheckMenuItem(FindID, MF_BYCOMMAND | MF_CHECKED);
				break;
			}
		}
	}
		break;
	case 1:
	{
		CString *m_MenuCheckTemp = (CString *)lParam;
		for (int i = 0; i < 42; i++)
		{
			if (m_MenuCheckTemp->Find(OptionsList[i].c_str()) > -1)
			{
				int FindID = i + ID_FILE_OPENDOWNLOADAGENTFILE;
				CMenu * menu = GetMenu();
				menu->CheckMenuItem(FindID, MF_BYCOMMAND | MF_UNCHECKED);
				break;
			}
		}
	}
	break;
	case 2:
	{
		COMPortType = lParam;
		UpdateData(false);
	}
		break;
	case 3:
	{
		CString *m_MenuCheckTemp = (CString *)lParam;
		for (int i = 0; i < 42; i++)
		{
			if (m_MenuCheckTemp->Find(OptionsList[i].c_str()) > -1)
			{
				int FindID = i + ID_FILE_OPENDOWNLOADAGENTFILE;
				CMenu * menu = GetMenu();
				menu->EnableMenuItem(FindID, MF_DISABLED | MF_BYCOMMAND);
				break;
			}
		}
	}
		break;
	case 4:
	{
		CString *m_MenuCheckTemp = (CString *)lParam;
		for (int i = 0; i < 42; i++)
		{
			if (m_MenuCheckTemp->Find(OptionsList[i].c_str()) > -1)
			{
				int FindID = i + ID_FILE_OPENDOWNLOADAGENTFILE;
				CMenu * menu = GetMenu();
				menu->EnableMenuItem(FindID, MF_ENABLED | MF_BYCOMMAND);
				break;
			}
		}
	}
		break;
	case 5:
	{
		GetDlgItem(IDC_UART_RD)->EnableWindow(false);
		GetDlgItem(IDC_USB_RD)->EnableWindow(false);
		GetDlgItem(IDC_UARTUSB_RD)->EnableWindow(false);
	}
		break;
	case 6:
	{
		GetDlgItem(IDC_UART_RD)->EnableWindow(true);
		GetDlgItem(IDC_USB_RD)->EnableWindow(true);
		GetDlgItem(IDC_UARTUSB_RD)->EnableWindow(true);
	}
		break;

	default:
		break;
	}
	return 0;
}

void DLOption::CCBInit()
{
	m_BaudCCB.ResetContent();
	m_BaudCCB.AddString("115200");
	m_BaudCCB.AddString("230400");
	m_BaudCCB.AddString("460800");
	m_BaudCCB.AddString("921600");
	m_DownloadTypeCCB.ResetContent();
	m_DownloadTypeCCB.AddString("Download ROM and FAT");
	m_DownloadTypeCCB.AddString("Download ROM");
	m_DownloadTypeCCB.AddString("Download ROM and Format");
	m_DownloadTypeCCB.AddString("Format");
}



void DLOption::OnBnClickedUartRd()
{
	UpdateData(true);
}


void DLOption::OnBnClickedAutoRd()
{
	UpdateData(true);
}


void DLOption::OnBnClickedNandRd()
{
	UpdateData(true);
}

void DLOption::DLTypeOption()
{
	CADCTDlg MainForm;
	COMPortType = MainForm.COMPortType;

	m_BaudCCB.SelectString(-1, MainForm.m_BaudCCB);
	//AfxMessageBox(MainForm.m_BaudCCB);
	m_DownloadTypeCCB.SelectString(-1, MainForm.m_DownloadTypeCCB);
	DownloadFormatOperation = MainForm.DownloadFormatOperation;
	FlashType = MainForm.FlashType;
	UpdateData(false);
}

void DLOption::SetPortCCB()
{
	CPublicFunction CPF;
	CPF.GetComList(&PortList);
	CString PortRes;

	m_PowerComCCB.ResetContent();
	for (int j = 0; j < PortList.GetSize(); j++)
	{
		PortRes = PortList.GetAt(j);
		m_PowerComCCB.AddString(PortRes);
	}

}

BOOL DLOption::PreTranslateMessage(MSG* pMsg)
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


//afx_msg LRESULT DLOption::OnDloption(WPARAM wParam, LPARAM lParam)
//{
//	return 0;
//}


afx_msg LRESULT DLOption::OnDloption(WPARAM wParam, LPARAM lParam)
{
	return 0;
}
