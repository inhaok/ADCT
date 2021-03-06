
// ADCTDlg.cpp: 实现文件
//

#include "stdafx.h"
#include <io.h>  
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include "direct.h"
#include "ADCT.h"
#include "ADCTDlg.h"
#include "afxdialogex.h"
#include "PublicFunction.h"
#include "SQLManager.h"
#include "FTPClass.h"
#include "WinRARClass.h"
#include "DLOption.h"
#include "ManagerLoginDlg.h"
#include "PowerManager.h"
#include "ThreadPool.h"
#include "ATOptionDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CString RadioAddress;
bool MultiDownloadIsOpen = false;
HWND MultiDownloadHWND = NULL;
HINSTANCE MultiDownloadHins;
HWND ATEHWND = NULL;
HWND AutoTestHWND = NULL;
HINSTANCE AutoTestHins;
HWND DLOptionHWND = NULL;
int StartPort = 0;
int COMPercentageArray[4] = { IDC_PORT1_LB ,IDC_PORT2_LB,IDC_PORT3_LB ,IDC_PORT4_LB };
CProgressCtrl PortStatusPG[4];
int PortStautsTB[4] = { IDC_PortStauts_TB1 ,IDC_PortStauts_TB2 ,IDC_PortStauts_TB3 ,IDC_PortStauts_TB4 };
int StartBtn[4] = { IDC_PortStart1_Btn ,IDC_PortStart2_Btn ,IDC_PortStart3_Btn ,IDC_PortStart4_Btn };
int StopBtn[4] = { IDC_PortStop1_Btn ,IDC_PortStop2_Btn ,IDC_PortStop3_Btn ,IDC_PortStop4_Btn };
int TipsLB[4] = { IDC_Tips_LB1 ,IDC_Tips_LB2 ,IDC_Tips_LB3 ,IDC_Tips_LB4 };
bool AutoTestInitStatus = false;
bool CheckFirstPortError[4] = { false,false, false, false };
int MultiDownloadErrorCount[4] = { 0 };
int TipsColor[4] = { 0 };
bool StopBit[4] = { true,true, true, true };
int PortUser[4] = { 0,0,0,0 };
int CountMessageStopBit[4] = { 0,0,0,0 };
CString SleepTimer;
bool FirstInitTimerCheck = false;


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CADCTDlg 对话框



CADCTDlg::CADCTDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ADCT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CADCTDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_PortStatus_PG1, PortStatusPG[0]);
	DDX_Control(pDX, IDC_PortStatus_PG2, PortStatusPG[1]);
	DDX_Control(pDX, IDC_PortStatus_PG3, PortStatusPG[2]);
	DDX_Control(pDX, IDC_PortStatus_PG4, PortStatusPG[3]);
	DDX_Control(pDX, IDC_LOG_RICHEDIT1, m_ctrlRichEditLog[0]);
	DDX_Control(pDX, IDC_LOG_RICHEDIT2, m_ctrlRichEditLog[1]);
	DDX_Control(pDX, IDC_LOG_RICHEDIT3, m_ctrlRichEditLog[2]);
	DDX_Control(pDX, IDC_LOG_RICHEDIT4, m_ctrlRichEditLog[3]);
	DDX_Control(pDX, IDC_Tips_LB1, m_TipsLB[0]);
	DDX_Control(pDX, IDC_Tips_LB2, m_TipsLB[1]);
	DDX_Control(pDX, IDC_Tips_LB3, m_TipsLB[2]);
	DDX_Control(pDX, IDC_Tips_LB4, m_TipsLB[3]);
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CADCTDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_SETTEXT, &CADCTDlg::OnSetTextMessage)
	ON_WM_COPYDATA()
	ON_MESSAGE(WM_SimpleMessage, &CADCTDlg::OnSimplemessage)
	ON_MESSAGE(WM_ADCTMainFormMessage, &CADCTDlg::OnMainformmessage)
	ON_BN_CLICKED(IDC_DLOption_Btn, &CADCTDlg::OnBnClickedDloptionBtn)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_PortStart1_Btn, IDC_PortStart4_Btn, OnSimplePortStartClick)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_PortStop1_Btn, IDC_PortStop4_Btn, OnSimplePortStopClick)
	ON_BN_CLICKED(IDC_PortAllStart_Btn, &CADCTDlg::OnBnClickedPortallstartBtn)
	ON_BN_CLICKED(IDC_PortAllStop_Btn, &CADCTDlg::OnBnClickedPortallstopBtn)
	ON_MESSAGE(WM_MasterManagerMessage, &CADCTDlg::OnMastermanagermessage)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON1, &CADCTDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CADCTDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_ATOption_Btn, &CADCTDlg::OnBnClickedAtoptionBtn)
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void InitConsoleWindow()
{
	AllocConsole();
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handle, _O_TEXT);
	FILE * hf = _fdopen(hCrt, "w");
	*stdout = *hf;
}


// CADCTDlg 消息处理程序

BOOL CADCTDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	//InitConsoleWindow();


	DLOption DLO;
	GetMainIniConfig();
	GetMultiDownloadIniConfig();
	MultiDownloadInit();//注意初始化与打开软件的顺序
	InitProgressControl();
	SetDlgItemText(IDC_OrderStatic_LB, OrderNameStatic);
	SetDlgItemText(IDC_VersionStatic_LB, VersionStatic);
	CommandList.SetSize(4);
	CommandListBackup.SetSize(4);
	for (int i = 0; i < 4; i++)
	{
		CommandList.SetAt(i, "");
		CommandListBackup.SetAt(i, "");
	}
	OpenMultiDownload();
	OpenAutoTest();
	/*PowerManager pm;
	pm.openPort(atoi(PowerCom.Right(PowerCom.GetLength() - 3)));
	for (int i = 0; i < 4; i++)
	{
		PowerSet(i, false);
	}
	pm.StartListen();*/
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CADCTDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CADCTDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HBRUSH CADCTDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	CFont m_font;
	m_font.CreateFont(40, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("宋体"));
	// TODO:  在此更改 DC 的任何特性
	for (int i = 0; i < 4; i++)
	{
		if (pWnd->GetDlgCtrlID() == TipsLB[i])
		{
			switch (TipsColor[i])
			{
			case 0:
			{
				pDC->SetTextColor(RGB(00, 00, 00));
			}
			break;
			case 1:
			{
				pDC->SetTextColor(RGB(255, 00, 00));
			}
			break;
			case 2:
			{
				pDC->SetTextColor(RGB(00, 255, 00));
			}
			break;
			case 3:
			{
				pDC->SetTextColor(RGB(00, 00, 255));
			}
			break;
			default:
			{
				pDC->SetTextColor(RGB(00, 00, 00));
			}
			break;
			}
			pDC->SelectObject(&m_font);
		}
	}


	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CADCTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CADCTDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SetMainIniConfig();
	//ADCTCopyDataSend(3, "ALL", 1, 6, "");
	::PostMessage(AutoTestHWND, WM_SimpleMessage, 3, 0);
	ADCTCopyDataSend(1, "ALL", 1, 99, "");
	
	CDialogEx::OnClose();
}
afx_msg LRESULT CADCTDlg::OnSetTextMessage(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{

	default:
		break;
	}
	return 0;
}
void CADCTDlg::OnMessageSend(int SendFor, int MessageNum, WPARAM WParam, LPARAM LParam)
{
	HWND SendForHWND = NULL;

	switch (SendFor)
	{
	case 0:
		SendForHWND = AfxGetMainWnd()->m_hWnd;
		break;
	case 1:
		SendForHWND = MultiDownloadHWND;
		break;
	case 2:
		SendForHWND = ATEHWND;
		break;
	case 3:
		SendForHWND = AutoTestHWND;
		break;
	default:
		break;
	}
	//CString temp;
	//temp.Format("%02x",SendForHWND);
	//AfxMessageBox("OnMessageSend"+temp);
	switch (MessageNum)
	{
	case WM_SimpleMessage:
		::SendMessageTimeout(SendForHWND, WM_SimpleMessage, WParam, LParam, SMTO_ABORTIFHUNG, 3000, 0);
		break;
	case WM_SETTEXT:
		::SendMessage(SendForHWND, WM_SETTEXT, WParam, LParam);
		break;
	case WM_COPYDATA:
		::SendMessageTimeout(SendForHWND, WM_COPYDATA, WParam, LParam,SMTO_ABORTIFHUNG,3000,0);
		break;
	default:
		break;
	}

}


void CADCTDlg::ADCTCopyDataSend(int SendFor, char  Port[1024], int Status, int MessageNum, char  MessageChar[1024])
{
	ADCTMessage *ADCTMsg = new ADCTMessage;
	strcpy_s(ADCTMsg->Port, Port);
	ADCTMsg->MessageNum = MessageNum;
	strcpy_s(ADCTMsg->MessageChar, MessageChar);
	//AfxMessageBox(MessageChar);
	//char* temp = "check";
	COPYDATASTRUCT cs;
	cs.dwData = Status;
	cs.cbData = sizeof(ADCTMessage);
	cs.lpData = (void *)ADCTMsg;
	OnMessageSend(SendFor, WM_COPYDATA, (WPARAM)GetSafeHwnd(), (LPARAM)&cs);
}

BOOL CADCTDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)//通讯回复处理
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CPublicFunction PF;
	DLOption DLO;
	ADCTMessage *m_ADCTMessage = new ADCTMessage;
	m_ADCTMessage = (ADCTMessage*)pCopyDataStruct->lpData;
	CString temp, PortStr, MessageCharStr;
	PortStr = m_ADCTMessage->Port;
	//AfxMessageBox(PortStr);
	MessageCharStr = m_ADCTMessage->MessageChar;
	/*temp.Format("Status:%d", pCopyDataStruct->dwData);
	AfxMessageBox(temp);
	temp.Format("MessageNum:%d", m_ADCTMessage->MessageNum);
	AfxMessageBox(temp);
	AfxMessageBox(m_ADCTMessage->MessageChar);*/

	/*char* temp = (char*)pCopyDataStruct->lpData;
	AfxMessageBox(temp);*/
	/*if (pWnd->m_hWnd ==MultiDownloadHWND)
	{
		MultiDownloadMessageReturn(pCopyDataStruct->dwData, m_ADCTMessage);
	}*/
	if (pWnd->m_hWnd == MultiDownloadHWND)
	{
		MultiDownloadCopyDataManage(PortStr, pCopyDataStruct, m_ADCTMessage);
	}
	else if (pWnd->m_hWnd == ATEHWND)
	{

	}
	else if (pWnd->m_hWnd == AutoTestHWND)
	{
		AutoTestCopyDataManage(pCopyDataStruct);
	}
	else
	{

	}

	return CDialogEx::OnCopyData(pWnd, pCopyDataStruct);
}

//多路下载的通讯处理  看协议
void CADCTDlg::MultiDownloadCopyDataManage(CString &PortStr, COPYDATASTRUCT * pCopyDataStruct, ADCTMessage * m_ADCTMessage)//后期优化  参数冗余
{
	if (PortStr == "ALL")
	{
		switch (pCopyDataStruct->dwData)
		{
		case 0:
		{

		}
		break;
		case 1:
		{
			switch (m_ADCTMessage->MessageNum)
			{
			case 0:
			{
				MultiDownloadIsOpen = true;
				SetMultiDownloadOption();

			}
			break;
			case 1:
			{

			}
			break;
			case 2:
			{
				char *GetChar = m_ADCTMessage->MessageChar;
				if (GetChar[0] != '\0')
				{

					DAFileBackup = GetChar;
				}
				else
				{
					DAFileBackup = "";
				}

				/*if (DLOptionHWND != NULL)
				{
					::SendMessage(DLOptionHWND, WM_DLOptionsMessage, 1, 0);
				}*/
			}
			break;
			case 5:
			{
				COMPortType = atoi(m_ADCTMessage->MessageChar);
				//AfxMessageBox(m_ADCTMessage->MessageChar);
			}
			break;
			case 6:
			{
				m_BaudCCB = m_ADCTMessage->MessageChar;
			}
			break;
			case 7:
			{
				m_DownloadTypeCCB = m_ADCTMessage->MessageChar;
			}
			break;
			case 8:
			{
				DownloadFormatOperation = atoi(m_ADCTMessage->MessageChar);
			}
			break;
			case 9:
			{
				FlashType = atoi(m_ADCTMessage->MessageChar);
				//DLO.DLTypeOption();
			}
			break;
			case 10:
			{

			}
			break;
			case 24:
			{
				LockOption();
			}
			break;
			case 25:
			{
				UnlockOption();
			}
			break;
			case 29:
			{
				CString CharTemp = m_ADCTMessage->MessageChar;
				::SendMessage(DLOptionHWND,WM_DLOptionsMessage, 0, (LPARAM)&CharTemp);
			}
			break;
			case 30:
			{
				CString CharTemp = m_ADCTMessage->MessageChar;
				::SendMessage(DLOptionHWND, WM_DLOptionsMessage, 1, (LPARAM)&CharTemp);
			}
			break;
			case 32:
			{
				::SendMessage(DLOptionHWND, WM_DLOptionsMessage,2, COMPortType);
			}
			break;
			case 33:
			{
				CString CharTemp = m_ADCTMessage->MessageChar;
				if (CharTemp== "m_Gb_ComPortType")
				{
					::SendMessage(DLOptionHWND, WM_DLOptionsMessage, 5, (LPARAM)&CharTemp);
				}
				else
				{
					::SendMessage(DLOptionHWND, WM_DLOptionsMessage, 3, (LPARAM)&CharTemp);
				}
				
			}
			break;
			case 34:
			{
				CString CharTemp = m_ADCTMessage->MessageChar;
				if (CharTemp == "m_Gb_ComPortType")
				{
					::SendMessage(DLOptionHWND, WM_DLOptionsMessage, 6, (LPARAM)&CharTemp);
				}
				else
				{
					::SendMessage(DLOptionHWND, WM_DLOptionsMessage,4, (LPARAM)&CharTemp);
				}

			}
			break;
			case 99:
			{
				MultiDownloadIsOpen = false;
				exit(0);
			}
			break;
			default:
				break;
			}
		}
		break;
		case 2:
		{
			switch (m_ADCTMessage->MessageNum)
			{
			case 0:
			{
				MultiDownloadIsOpen = true;
				ADCTCopyDataSend(1, "ALL", 0, 1, "");
				ADCTCopyDataSend(1, "ALL", 1, 3, MultiDownloadFilePath.GetBuffer());//文件路径传输
			}
			break;
			case 1:
			{
				int NowPort;
				CString m_NowPort;
				StartPort = atoi(m_ADCTMessage->MessageChar);
				for (int i = 0; i < 4; i++)
				{
					NowPort = StartPort + i;
					m_NowPort.Format("COM%d", NowPort);
					GetDlgItem(COMPercentageArray[i])->SetWindowText(m_NowPort);
				}

			}
			break;
			default:
				break;
			}
		}
		break;
		default:
			break;
		}
	}
	else
	{
		CString GetPortIndex = PortStr;
		GetPortIndex = GetPortIndex.Right(GetPortIndex.GetLength() - 3);
		int PortIndex = atoi(GetPortIndex) - StartPort;
		if (PortIndex > 4)
		{
			return;
		}
		switch (pCopyDataStruct->dwData)
		{
		case 0:
		{
		}
		break;
		case 1:
		{
			switch (m_ADCTMessage->MessageNum)
			{
			case 10:
			{
				if (!StopBit[PortIndex])
				{
					int Pos = atoi(m_ADCTMessage->MessageChar);
					PortStatusPG[PortIndex].SetPos(Pos);
				}
				
				/*if (Pos == 0 && PortStautsTB[PortIndex])
				{

				}*/

			}
			break;
			case 11:
			{
				if (!StopBit[PortIndex])
				{
					CString CharTemp = m_ADCTMessage->MessageChar;
					if (CharTemp == "ERROR")
					{
						break;
					}
					GetDlgItem(PortStautsTB[PortIndex])->SetWindowText(CharTemp);
					if (CharTemp == "START")
					{
						PortUser[PortIndex] = 1;
						SendMessage(WM_MasterManagerMessage, MessagePowerON, PortIndex);
					}
				}
			}
			break;
			case 12:
			{
				//颜色传输处理  BCB对应BGR处理  勿改
				CString CharTemp = m_ADCTMessage->MessageChar;
				COLORREF ColorTemp = strtoul(CharTemp, NULL, 16);
				PortStatusPG[PortIndex].SetBarColor(ColorTemp);
				CString CheckText;
				GetDlgItem(PortStautsTB[PortIndex])->GetWindowText(CheckText);
				if (CheckText == "START")
				{
					CheckFirstPortError[PortIndex] = true;
					ShowLog(PortIndex, "烧写开始", 3);
					TipsColor[PortIndex] = 0;
					m_TipsLB[PortIndex].SetWindowText("烧写中");
				}
			}
			break;
			case 26:
			{
				GetDlgItem(StartBtn[PortIndex])->EnableWindow(false);
			}
			break;
			case 27:
			{
				if (!CheckFirstPortError[PortIndex])
				{
					GetDlgItem(StartBtn[PortIndex])->EnableWindow(true);
				}
			}
			break;
			case 28:
			{
				CString CharTemp = m_ADCTMessage->MessageChar;
				if (CharTemp == "Success")
				{
					ShowLog(PortIndex, "烧写完成，开始测试", 3);
					MultiDownloadErrorCount[PortIndex] = 0;
					GetDlgItem(PortStautsTB[PortIndex])->SetWindowText("烧写完成");
					SendMessage(WM_MasterManagerMessage, MessagePowerOFF, PortIndex);
					//Sleep(atoi(SleepTimer));
					SendMessage(WM_MasterManagerMessage, MessagePowerON, PortIndex);
					SendMessage(WM_MasterManagerMessage, MessageMultiDownloadSuccess, (LPARAM)&PortStr);
				}
				else if (CharTemp == "STOP")
				{
					CountMessageStopBit[PortIndex]++;
					CheckStopCount(PortIndex);
					//SendMessage(WM_MasterManagerMessage, MessageMultiDownloadSTOP, (LPARAM)&PortStr);
					
				}
				else if (CharTemp == "ERROR")
				{
					if (CheckFirstPortError[PortIndex] && MultiDownloadErrorCount[PortIndex] < 1)
					{
						SendMessage(WM_MasterManagerMessage, MessageMultiDownloadRestart, (LPARAM)&PortStr);
						//Sleep(1000);

						ShowLog(PortIndex, "烧写失败，重新开始烧写", 3);
						MultiDownloadErrorCount[PortIndex]++;
						SendMessage(WM_MasterManagerMessage, MessagePowerOFF, PortIndex);
						//Sleep(atoi(SleepTimer));
						SendMessage(WM_MasterManagerMessage, MessagePowerON, PortIndex);
						break;
					}
					else if (CheckFirstPortError[PortIndex] && MultiDownloadErrorCount[PortIndex] == 1)
					{
						ShowLog(PortIndex, "烧写失败过多，请检测串口", 0);
						SimplePortBtn(1, 0, PortStr.GetBuffer());
					}

					//SendMessage(WM_MasterManagerMessage, MessageMultiDownloadSTOP, (LPARAM)&PortStr);
				}

			}
			break;
			
			default:
				break;
			}
		}
		break;
		default:
			break;
		}
	}
}

void CADCTDlg::AutoTestCopyDataManage(COPYDATASTRUCT* pCopyDataStruct)
{
	ADCTMessage *m_ADCTMessage = new ADCTMessage;
	m_ADCTMessage = (ADCTMessage*)pCopyDataStruct->lpData;
	CString Port = m_ADCTMessage->Port;
	int Status = pCopyDataStruct->dwData;
	int MessageNum = m_ADCTMessage->MessageNum;
	CString MessageChar = m_ADCTMessage->MessageChar;
	if (Port == "ALL")
	{
		switch (Status)
		{
		case 0:
			break;
		case 1:
		{
			switch (MessageNum)
			{
			case 0:
			{
				if (MessageChar == "1")
				{
					AutoTestInitStatus = true;
				}
				else
				{
					AutoTestInitStatus = false;
					AfxMessageBox("自动测试配置获取失败，请检测订单号或版本号是否有误");
				}
			}

			break;
			case 4:
			{
				if (MessageChar == "OK")
				{
					CString Temp1, Temp2, OrderAndVersion;
					GetDlgItem(IDC_OrderStatic_LB)->GetWindowText(Temp1);
					GetDlgItem(IDC_VersionStatic_LB)->GetWindowText(Temp2);
					OrderAndVersion = Temp1 + "@@" + Temp2;
					ADCTCopyDataSend(3, "ALL", 1, 0, OrderAndVersion.GetBuffer());
				}
			}
			break;
			case 6:
			{
				if (MessageChar == "99")
				{

				}
				else if (MessageChar == "98")
				{

				}
			}
			break;
			default:
				break;
			}
		}
		break;
		default:
			break;
		}
	}
	else
	{
		CString GetPortIndex = Port;
		GetPortIndex = GetPortIndex.Right(GetPortIndex.GetLength() - 3);
		int PortIndex = atoi(GetPortIndex) - StartPort;
		switch (Status)
		{
		case 0:
		{
			switch (MessageNum)
			{
			case 0:
			{
				if (MessageChar == "失败")
				{
					TipsColor[PortIndex] = 1;
					//SendMessage(WM_MasterManagerMessage, MessagePowerOFF, PortIndex);
				}
				else if (MessageChar == "成功" || MessageChar == "已测试")
				{
					TipsColor[PortIndex] = 2;
					//SendMessage(WM_MasterManagerMessage, MessagePowerOFF, PortIndex);
				}
				else if (MessageChar == "测试")
				{
					TipsColor[PortIndex] = 0;
				}
				else
				{
					TipsColor[PortIndex] = 3;
				}
				m_TipsLB[PortIndex].SetWindowText(MessageChar);
				TipsColor[PortIndex] = 0;
			}
			break;
			case 1:
			{
				ShowLog(PortIndex, MessageChar, 3);
			}
			break;
			case 2:
			{
				ShowLog(PortIndex, MessageChar, 0);
			}
			break;
			case 3:
			{
				AfxMessageBox(MessageChar);
			}
			break;
			default:
				break;
			}
		}
		break;
		case 1:
		{
			switch (MessageNum)
			{
			case 1://串口开始，确认回复
			{
				if (MessageChar == "1")
				{
					PortUser[PortIndex] = 3;
					GetDlgItem(PortStautsTB[PortIndex])->SetWindowText("自动测试中");
					PortStatusPG[PortIndex].SetBarColor(RGB(255,255,255));
					GetDlgItem(StartBtn[PortIndex])->EnableWindow(false);

					SendMessage(WM_MasterManagerMessage, MessagePowerON, PortIndex);
				}
				else
				{
					GetDlgItem(StartBtn[PortIndex])->EnableWindow(true);
				}
			}
			break;
			case 2:
			{
				if (MessageChar == "1")
				{
					//GetDlgItem(PortStautsTB[PortIndex])->SetWindowText("停止");
					CountMessageStopBit[PortIndex]++;
					CheckStopCount(PortIndex);
					//GetDlgItem(StartBtn[PortIndex])->EnableWindow(true);
				}
				else
				{
					/*GetDlgItem(StartBtn[PortIndex])->EnableWindow(true);*/   //停止失败
				}
			}
			break;
			case 3:
			{
				if (MessageChar == "1")
				{
					/*TipsColor[PortIndex] = 2;
					m_TipsLB[PortIndex].SetWindowText("成功");
					TipsColor[PortIndex] = 0;*/
					SendMessage(WM_MasterManagerMessage, MessageAutoTestSuccess, (LPARAM)&Port);
				}
				else
				{
					/*TipsColor[PortIndex] = 1;
					m_TipsLB[PortIndex].SetWindowText("失败");
					TipsColor[PortIndex] = 0;*/
					SendMessage(WM_MasterManagerMessage, MessageAutoTestERROR, (LPARAM)&Port);
				}
			}
			break;
			default:
				break;
			}
		}
		break;
		default:
			break;
		}
	}
}

afx_msg LRESULT CADCTDlg::OnSimplemessage(WPARAM wParam, LPARAM lParam)
{
	/*CString temp;
	temp.Format("%02x", lParam);
	AfxMessageBox("OnSimplemessage:"+temp);*/
	switch (wParam)
	{
	case 1:
	{
		MultiDownloadHWND = (HWND)lParam;
		Sleep(10);
		ADCTCopyDataSend(1, "ALL", 0, 0, "");
	}
	break;
	case 2:
	{
		ATEHWND = (HWND)lParam;
		Sleep(10);

	}
	break;
	case 3:
	{
		switch (lParam)
		{
		case 99:
		{
			exit(0);
		}
		break;
		default:
		{
			AutoTestHWND = (HWND)lParam;
			Sleep(10);
			GetDlgItem(IDC_ADCTInitStatus_LB)->SetWindowText("初始化完成");
			for (int i = 0; i < 4; i++)
			{
				GetDlgItem(StartBtn[i])->EnableWindow(true);
				GetDlgItem(StopBtn[i])->EnableWindow(true);
				GetDlgItem(IDC_PortAllStart_Btn)->EnableWindow(true);
				GetDlgItem(IDC_PortAllStop_Btn)->EnableWindow(true);
			}
			SetTimer(CleanInitLog, 3000, NULL);
			ADCTCopyDataSend(3, "ALL", 0, 1, "ALL");
		}
		break;
		}
	}
	break;
	default:
		break;
	}
	return 0;
}

void CADCTDlg::MultiDownloadInit()
{
	CPublicFunction pf;
	CString sCurPath = pf.GetCurDir();
	CString MultiDownloadIni = sCurPath + "\\MultiDownload\\MultiPortFlashDownloadProject.ini";
	CString DABinFile = sCurPath + "\\MultiDownload\\MTK_AllInOne_DA.bin";
	WritePrivateProfileString("form", "DA_bin", DABinFile.GetBuffer(), MultiDownloadIni);
	DAFileBackup = DABinFile;
}

void CADCTDlg::AutoTestInit()
{

}


void CADCTDlg::LockOption()
{
	GetDlgItem(IDC_DLOption_Btn)->EnableWindow(false);
	GetDlgItem(IDC_ATOption_Btn)->EnableWindow(false);
}

void CADCTDlg::UnlockOption()
{
	GetDlgItem(IDC_DLOption_Btn)->EnableWindow(true);
	GetDlgItem(IDC_ATOption_Btn)->EnableWindow(true);
}


afx_msg LRESULT CADCTDlg::OnMainformmessage(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{

	default:
		break;
	}
	return 0;
}

void CADCTDlg::GetMainIniConfig()
{
	CPublicFunction pf;
	CString sCurPath = pf.GetCurDir();
	CFileStatus Status;
	CString m_sIniFile = sCurPath + "ADCT.ini";

	if (!CFile::GetStatus(m_sIniFile, Status))           //检测不到，创建默认配置
	{
		WritePrivateProfileString("PowerOption", "COM", "6", m_sIniFile);
		WritePrivateProfileString("PowerOption", "RadioAddress", "254", m_sIniFile);
		WritePrivateProfileString("PowerOption", "SleepTimer", "1000", m_sIniFile);
		PowerCom = "COM6";
		RadioAddress = "254";
		SleepTimer = "1000";
	}
	else
	{
		CString strTemp;

		GetPrivateProfileString("PowerOption", "COM", NULL, strTemp.GetBuffer(128), 128, m_sIniFile);
		PowerCom.Format("COM%s", strTemp);
		GetPrivateProfileString("PowerOption", "RadioAddress", NULL, strTemp.GetBuffer(128), 128, m_sIniFile);
		RadioAddress = strTemp;
		GetPrivateProfileString("PowerOption", "SleepTimer", NULL, strTemp.GetBuffer(128), 128, m_sIniFile);
		SleepTimer = strTemp;
		strTemp.ReleaseBuffer(128);
	}
}

void CADCTDlg::SetMainIniConfig()
{
	CPublicFunction pf;
	CString sCurPath = pf.GetCurDir();
	CFileStatus Status;
	CString m_sIniFile = sCurPath + "ADCT.ini";
	WritePrivateProfileString("PowerOption", "COM", PowerCom.Right(PowerCom.GetLength() - 3), m_sIniFile);
	WritePrivateProfileString("PowerOption", "RadioAddress", RadioAddress, m_sIniFile);
	WritePrivateProfileString("PowerOption", "SleepTimer", SleepTimer, m_sIniFile);
}

void CADCTDlg::GetMultiDownloadIniConfig()
{
	CPublicFunction pf;
	CString sCurPath = pf.GetCurDir();
	CFileStatus Status;
	CString m_sIniFile = sCurPath + "\\MultiDownload\\MultiPortFlashDownloadProject.ini";
	if (!CFile::GetStatus(m_sIniFile, Status))           //检测不到，创建默认配置
	{
		//WritePrivateProfileString("SQLSetting", "SQLAddress", "120.77.221.153", m_sIniFile);
	}
	else
	{
		CString strTemp;

		GetPrivateProfileString("form", "Type", NULL, strTemp.GetBuffer(128), 128, m_sIniFile);
		//AfxMessageBox(strTemp);
		strTemp.ReleaseBuffer(128);
	}
}

void CADCTDlg::OnBnClickedDloptionBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	ManagerLoginDlg m_ManagerDlg;
	INT_PTR LoginRes;
	LoginRes = m_ManagerDlg.DoModal();
	if (LoginRes == IDOK)
	{

		INT_PTR Res;
		DLOption DLODlg;
		DLODlg.COMPortType = COMPortType;
		DLODlg.m_Baud = m_BaudCCB;
		DLODlg.m_DownloadType = m_DownloadTypeCCB;
		DLODlg.DownloadFormatOperation = DownloadFormatOperation;
		DLODlg.FlashType = FlashType;
		DLODlg.DownloadFilePathBackup = MultiDownloadFilePath;
		DLODlg.m_PowerComText = PowerCom;
		DLODlg.m_RadioAddressText = RadioAddress;
		DLODlg.m_SleepTimer = SleepTimer;
		Res = DLODlg.DoModal();
		if (Res == IDOK)
		{
			COMPortType = DLODlg.COMPortType;
			m_BaudCCB = DLODlg.m_Baud;
			m_DownloadTypeCCB = DLODlg.m_DownloadType;
			DownloadFormatOperation = DLODlg.DownloadFormatOperation;
			FlashType = DLODlg.FlashType;
			PowerCom = DLODlg.m_PowerComText;
			RadioAddress = DLODlg.m_RadioAddressText;
			SleepTimer = DLODlg.m_SleepTimer;
			//AfxMessageBox(m_BaudCCB);
			SendOptionForMultiDownload();

		}
	}

}

void CADCTDlg::SetMultiDownloadOption()
{
	CPublicFunction pf;
	CString m_MultiDownloadIniPath = pf.GetCurDir() + "MultiDownload\\MultiPortFlashDownloadProject.ini";
	CString m_MultiDownloadDAPath = pf.GetCurDir() + "MultiDownload\\MTK_AllInOne_DA.bin";
	ADCTCopyDataSend(1, "ALL", 1, 2, m_MultiDownloadDAPath.GetBuffer());

}

void CADCTDlg::CheckToolStatus()
{
	ADCTCopyDataSend(1, "ALL", 0, 0, "");
}
void CADCTDlg::SendOptionForMultiDownload()
{
	CString IntTemp = "";
	IntTemp.Format("%d", COMPortType);
	ADCTCopyDataSend(1, "ALL", 1, 5, IntTemp.GetBuffer());
	ADCTCopyDataSend(1, "ALL", 1, 6, m_BaudCCB.GetBuffer());
	ADCTCopyDataSend(1, "ALL", 1, 7, m_DownloadTypeCCB.GetBuffer());
	IntTemp.Format("%d", DownloadFormatOperation);
	ADCTCopyDataSend(1, "ALL", 1, 8, IntTemp.GetBuffer());
	IntTemp.Format("%d", FlashType);
	ADCTCopyDataSend(1, "ALL", 1, 9, IntTemp.GetBuffer());
}

//0开始 1停止
void CADCTDlg::SimplePortBtn(int Operate, int PortIndex, char *Port) //Index有空再写 
{
	switch (Operate)
	{
	case 0:
	{

		Sleep(100);
		ADCTCopyDataSend(1, Port, 1, 20, "");

	}
	break;
	case 1:
	{

		Sleep(100);
		for (int i = 0; i < 2; i++)
		{
			ADCTCopyDataSend(1, Port, 1, 21, "");
		}
		ADCTCopyDataSend(3, Port, 1, 2, "");
	}
	break;
	default:
		break;
	}

}

void CADCTDlg::OnSimplePortStartClick(UINT nID)
{
	int PortIndex = nID - IDC_PortStart1_Btn;
	StopBit[PortIndex] = false;
	
	CString ComPort;
	ComPort.Format("COM%d", PortIndex + StartPort);
	if (ListenStartBtn(1))
	{
		MultiDownloadErrorCount[PortIndex] = 0;
	}
	SimplePortBtn(0, PortIndex, ComPort.GetBuffer());
}
void CADCTDlg::OnSimplePortStopClick(UINT nID)
{
	int PortIndex = nID - IDC_PortStop1_Btn;
	CountMessageStopBit[PortIndex] = 0;
	StopBit[PortIndex] = true;
	CString ComPort;
	ComPort.Format("COM%d", PortIndex + StartPort);
	MultiDownloadErrorCount[PortIndex] = 0;
	SimplePortBtn(1, PortIndex, ComPort.GetBuffer());
	if (!GetDlgItem(StartBtn[PortIndex])->IsWindowEnabled())
	{
		GetDlgItem(PortStautsTB[PortIndex])->SetWindowText("正在停止串口");
		//GetDlgItem(nID)->EnableWindow(true);
	}
	else
	{
		GetDlgItem(PortStautsTB[PortIndex])->SetWindowText("停止");
	}
	ListenStartBtn(2);
}


void CADCTDlg::OnBnClickedPortallstartBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	//电源控制


	//GetDlgItem(IDC_PortAllStart_Btn)->EnableWindow(false);
	for (int i = 0; i < 4; i++)
	{
		if (GetDlgItem(StartBtn[i])->IsWindowEnabled())
		{
		}
		else
		{
			AfxMessageBox("串口未完全停止，无法全部重新开始");
			return;
		}
	}
	if (ListenStartBtn(0))
	{
		for (int i = 0; i < 4; i++)
		{
			
			CheckFirstPortError[i] = false;
			StopBit[i] = false;
			MultiDownloadErrorCount[i] = 0;
		}
		ADCTCopyDataSend(1, "ALL", 1, 22, "");
	}
	

}


void CADCTDlg::OnBnClickedPortallstopBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	ADCTCopyDataSend(1, "ALL", 1, 23, "");
	ADCTCopyDataSend(3, "ALL", 1, 2, "ALL");
	s_bExit = true;
	m_SendThread = true;
	m_ListenThread = true;
	for (int i = 0; i < 4; i++)
	{
		if (!GetDlgItem(StartBtn[i])->IsWindowEnabled())
		{
			GetDlgItem(PortStautsTB[i])->SetWindowText("正在停止串口");
		}
		else
		{
			GetDlgItem(PortStautsTB[i])->SetWindowText("停止");
		}
		CountMessageStopBit[i] = 0;
		CString ComPort;
		ComPort.Format("COM%d", i + StartPort);
		SimplePortBtn(1, NULL, ComPort.GetBuffer());
		StopBit[i] = true;
		MultiDownloadErrorCount[i] = 0;
		CommandList.SetAt(i, "");
	}
	//ListenStartBtn(2);
	GetDlgItem(IDC_PortAllStart_Btn)->EnableWindow(true);

	PowerManager pm;
	pm.ClosePort();
}

void CADCTDlg::InitProgressControl()
{
	for (int i = 0; i < 4; i++)
	{
		PortStatusPG[i].SetBkColor(RGB(255, 255, 255));
	}
}

void CADCTDlg::MultiDownloadManager()
{

}

afx_msg LRESULT CADCTDlg::OnMastermanagermessage(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case MessagePowerOFF:
	{
		if (lParam == -1)
		{
			for (int i = 0; i < 4; i++)
			{
				PowerSet(i, false);
			}
		}
		else
		{
			PowerSet(lParam, false);
		}

	}
	break;
	case MessagePowerON:
	{
		if (lParam == -1)
		{
			for (int i = 0; i < 4; i++)
			{
				PowerSet(i, true);
			}
		}
		else
		{
			PowerSet(lParam, true);
		}
	}
	break;
	case MessageMultiDownloadSuccess:
	{
		CString *Port = (CString *)lParam;
		if (StopBit[COMPortToIndex(Port->GetBuffer())])
		{
			break;
		}
		SimplePortBtn(1, 0, Port->GetBuffer());
		ADCTCopyDataSend(3, Port->GetBuffer(), 1, 1, "ALL");
	}
	break;
	case MessageMultiDownloadERROR:
	{

	}
	break;
	case MessageMultiDownloadSTOP:
	{

	}
	break;
	case MessageMultiDownloadRestart:
	{
		CString *Port = (CString *)lParam;
		if (StopBit[COMPortToIndex(Port->GetBuffer())])
		{
			break;
		}
		SimplePortBtn(1, 0, Port->GetBuffer());
		Sleep(1000);
		SimplePortBtn(0, 0, Port->GetBuffer());
	}
	break;
	case MessageAutoTestSuccess:
	{
		CString *Port = (CString *)lParam;
		if (StopBit[COMPortToIndex(Port->GetBuffer())])
		{
			break;
		}
		SimplePortBtn(0, 0, Port->GetBuffer());

	}
	break;
	case MessageAutoTestERROR:
	{
		CString *Port = (CString *)lParam;
		if (StopBit[COMPortToIndex(Port->GetBuffer())])
		{
			break;
		}
		SimplePortBtn(0, 0, Port->GetBuffer());
	}
	break;
	case MessageAutoTestSTOP:
		break;
	default:
		break;
	}
	return 0;
}

/*
Address:控制的继电器的地址 特殊：-1查询继电器 -2全开全断
PowerAction:开关动作
*/
void CADCTDlg::PowerSet(int Address, bool PowerAction)
{
	CPublicFunction pf;
	CString HexTemp, SendText, DictateNum;
	int HexLen = 1;
	unsigned short CrcTemp;
	SendText.Format("%X", atoi(RadioAddress));
	if (Address == -1)             //暂时不做
	{
		HexTemp = "01";
		HexLen++;
		return;
	}
	else if (Address == -2)		 //暂时不做
	{
		HexTemp = "0F";
		HexLen++;
		return;
	}
	else if (Address > -1 && Address < 32)
	{
		HexTemp.Format("0500%02X", Address);
		HexLen += 3;
	}
	else
	{
		return;
	}
	SendText += HexTemp;
	if (PowerAction)
	{
		HexTemp = "FF00";
	}
	else
	{
		HexTemp = "0000";
	}
	HexLen += 2;
	SendText += HexTemp;
	BYTE * HexTemp2;
	HexTemp2 = pf.CStrToByte(6, SendText);
	//unsigned char buff[6] = { 0 };
	//buff[0] = 0xFE;
	//buff[1] = 0x05;
	//buff[2] = 0x00;
	//buff[3] = 0x00;
	//buff[4] = 0x00;
	//buff[5] = 0x00;
	HexTemp = pf.UART_CRC16_Work(HexTemp2, 6);
	SendText += HexTemp;
	CommandListSet(Address, SendText);
}

void CADCTDlg::CommandListSet(int At, CString str)
{
	if (CommandList.GetAt(At) == "")
	{
		CommandList.SetAt(At, str);
	}
	else if (CommandListBackup.GetAt(At) == "")
	{
		CommandListBackup.SetAt(At, str);
	}
	else
	{
		return;
	}
}

//0红色1绿色2橙色3黑色
void CADCTDlg::ShowLog(int IndexControl, CString szLog, int TextColor)
{
	CHARFORMAT cf;
	switch (TextColor)
	{
	case 0:
	{
		cf.crTextColor = RGB(255, 0, 0);
	}
	break;
	case 1:
	{
		cf.crTextColor = RGB(0, 255, 0);
	}
	break;
	case 2:
	{
		cf.crTextColor = RGB(255, 165, 0);
	}
	break;
	case 3:
	{
		cf.crTextColor = RGB(0, 0, 0);
	}
	break;
	default:
		break;
	}
	try
	{
		if (IndexControl<0|| IndexControl>3)
		{
			return;
		}
		cf.dwMask = CFM_BOLD | CFM_COLOR | CFM_FACE |
			CFM_ITALIC | CFM_SIZE | CFM_UNDERLINE;
		cf.dwEffects = 0;
		cf.yHeight = 14 * 14;//文字高度
		strcpy_s(cf.szFaceName, _T("宋体"));//设置字体
		CString szTemp;
		CString szTime;
		CTime cur = CTime::GetCurrentTime();
		CString dayTime = cur.Format("%Y-%m-%d");
		szTime = cur.Format("%H:%M:%S");
		szTemp.Format(_T("%s %s\r\n"), szTime.GetBuffer(), szLog.GetBuffer());
		m_ctrlRichEditLog[IndexControl].SetSel(-1, -1);
		m_ctrlRichEditLog[IndexControl].SetSelectionCharFormat(cf);
		m_ctrlRichEditLog[IndexControl].ReplaceSel(szTemp);
		m_ctrlRichEditLog[IndexControl].PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		CString ComTemp;
		ComTemp.Format("%d", IndexControl + StartPort);
		CString logFileName = ".\\LOG\\" + dayTime + "COM" + ComTemp + "LOG.txt";
		char *fileName = logFileName.GetBuffer(), *tag;
		for (tag = fileName; *tag; tag++)
		{
			if (*tag == '\\')
			{
				char buf[1000], path[1000];
				strcpy_s(buf, fileName);
				buf[strlen(fileName) - strlen(tag) + 1] = NULL;
				strcpy_s(path, buf);
				if (_access(path, 6) == -1)
				{
					_mkdir(path);
				}
			}
		}
		ofstream out(".\\LOG\\" + dayTime + "COM" + ComTemp + "LOG.txt", ios::app);
		if (out.is_open())
		{
			out << szTemp;
			out.close();
		}
	}
	catch (const std::exception& e)
	{
		AfxMessageBox(e.what());
	}
	
}

void CADCTDlg::OpenMultiDownload()
{
	HWND  MultiDownloadHwndTemp = ::FindWindow("TMainForm", "Multiple Download Tool - Engineer mode");
	if (MultiDownloadHwndTemp != NULL)
	{
		::SendMessage(MultiDownloadHwndTemp, WM_CLOSE, 0, 0);
	}
	if (MultiDownloadHWND == NULL)
	{
		CPublicFunction *CPF = new CPublicFunction();
		string temp = CPF->GetCurDir() + "\\MultiDownload\\MultiPortFlashDownloadProject.exe";
		MultiDownloadHins = ShellExecute(NULL, "open", temp.c_str()
			, NULL, NULL, SW_SHOW);

	}
	else
	{

		::SendMessage(MultiDownloadHWND, WM_CLOSE, 0, 0);
		MultiDownloadHWND = NULL;
		Sleep(2000);
		OpenMultiDownload();
	}
}

void CADCTDlg::OpenAutoTest()
{
	HWND  AutoTestHwndTemp = ::FindWindow("", "自动测试v1.0.5.3(10)");
	if (AutoTestHwndTemp != NULL)
	{
		::SendMessage(AutoTestHwndTemp, WM_CLOSE, 0, 0);
	}
	AutoTestHwndTemp = ::FindWindow("", "MultComTest");
	if (AutoTestHwndTemp != NULL)
	{
		::SendMessage(AutoTestHwndTemp, WM_CLOSE, 0, 0);
	}
	AutoTestHwndTemp = ::FindWindow("", "BT_AMC/WIFI_MAC/IMEI_WRITER");
	if (AutoTestHwndTemp != NULL)
	{
		::SendMessage(AutoTestHwndTemp, WM_CLOSE, 0, 0);
	}
	if (AutoTestHWND == NULL)
	{
		CPublicFunction *CPF = new CPublicFunction();
		string temp = CPF->GetCurDir() + "\\自动测试工具\\MultComTest.exe";
		MultiDownloadHins = ShellExecute(NULL, "open", temp.c_str()
			, NULL, NULL, SW_SHOW);

	}
	else
	{

		::SendMessage(AutoTestHWND, WM_CLOSE, 0, 0);
		AutoTestHWND = NULL;
		Sleep(2000);
		OpenAutoTest();
	}
}



BOOL CADCTDlg::PreTranslateMessage(MSG* pMsg)
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

CString CADCTDlg::CutStringc(CString str)   //自动转行
{
	int len;
	len = str.GetLength();
	CString temp_str;
	temp_str = "";
	int templen = 0;
	for (int i = 0; i < len; i++)
	{
		temp_str += str[i];
		int aaa = (int)str.GetAt(i);
		if (aaa > 255)
		{
			templen += 2;
		}
		else
		{
			templen += 1;
		}
		if (templen > 8)
		{
			CString strLineFeed = _T("\r\n");
			temp_str = temp_str + strLineFeed;
			templen = 0;
		}
	}
	return temp_str;
}


void CADCTDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString numtemp;
	GetDlgItem(IDC_NUM_TB)->GetWindowText(numtemp);
	int Indextemp = atoi(numtemp);
	SendMessage(WM_MasterManagerMessage, MessagePowerON, Indextemp);
}


void CADCTDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString numtemp;
	GetDlgItem(IDC_NUM_TB)->GetWindowText(numtemp);
	int Indextemp = atoi(numtemp);
	SendMessage(WM_MasterManagerMessage, MessagePowerOFF, Indextemp);
}

bool CADCTDlg::ThreadManager(bool active)
{
	PowerManager pm;

	if (active)
	{

		if (pm.openPort(atoi(PowerCom.Right(PowerCom.GetLength() - 3))))
		{
			for (int i = 0; i < 4; i++)
			{
				PowerSet(i, false);
			}
			pm.StartListen();
			return true;
		}
		else
		{
			//AfxMessageBox("")
			return false;
		}
	}
	else
	{
		if (CheckStartStatus())
		{
			for (int i = 0; i < 4; i++)
			{
				CommandList[i] = "";
				CommandListBackup[i] = "";
			}
			s_bExit = true;
			m_SendThread = true;
			m_ListenThread = true;
			pm.ClosePort();
			return true;
		}
		else
		{
			AfxMessageBox("有串口未被关闭，线程停止失败");
			return false;
		}
	}


}

bool CADCTDlg::CheckStartStatus()
{
	for (int i = 0; i < 4; i++)
	{
		if (!GetDlgItem(StartBtn[i])->IsWindowEnabled())
		{
			return false;
		}

	}
	return true;
}

bool CADCTDlg::ListenStartBtn(int BtnControl)
{
	bool ThreadRes=false;
	switch (BtnControl)
	{
	case 0:
	{
		if (s_bExit)
		{
			ThreadRes=ThreadManager(true);
		}
		return true;
	}
	break;
	case 1:
	{
		if (s_bExit)
		{
			ThreadRes=ThreadManager(true);
		}
		return true;
	}
	break;
	case 2:
	{
		for (int i = 0; i < 4; i++)
		{
			if (!StopBit[i])
			{
				/*CString temp;
				temp.Format("串口%d未停止，关闭线程失败", i + 1);
				AfxMessageBox(temp);*/
				return false;
			}
		}
		if (s_bExit)
		{
			return false;
		}
		ThreadRes=ThreadManager(false);
	}
	break;
	case 3:
	{
		ThreadRes=ThreadManager(false);
	}
	break;
	default:
		break;
	}
	return ThreadRes;
}

int CADCTDlg::COMPortToIndex(CString COMPort)
{
	int COMPortToInt = atoi(COMPort.Right(COMPort.GetLength() - 3));
	int IndexStr = COMPortToInt - StartPort;
	return IndexStr;
}

void CADCTDlg::OnBnClickedAtoptionBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	ManagerLoginDlg m_ManagerDlg;
	INT_PTR LoginRes;
	LoginRes = m_ManagerDlg.DoModal();
	if (LoginRes == IDOK)
	{

		INT_PTR Res;
		ATOptionDlg ATOptionDLG;
		ATOptionDLG.m_ATPortType = ATPortType;
		ATOptionDLG.m_ATReTest = ATReTest;
		ATOptionDLG.m_ATTD = ATTD;
		Res = ATOptionDLG.DoModal();
		if (Res == IDOK)
		{
			ATPortType = ATOptionDLG.m_ATPortType;
			ATReTest = ATOptionDLG.m_ATReTest;
			ATTD = ATOptionDLG.m_ATTD;
			CString SendTemp;
			SendTemp.Format("%d", ATReTest);
			ADCTCopyDataSend(3, "ALL", 1, 3, SendTemp.GetBuffer());
			SendTemp.Format("%d", ATTD);
			ADCTCopyDataSend(3, "ALL", 1, 4, SendTemp.GetBuffer());
			if (ATPortType=="终端---稳定端口")
			{
				SendTemp = "0";
			}
			else
			{
				SendTemp = "1";
			}
			ADCTCopyDataSend(3, "ALL", 1, 5, SendTemp.GetBuffer());
		}
	}
}


void CADCTDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case CleanInitLog:
	{
		if (!FirstInitTimerCheck)
		{
			FirstInitTimerCheck = true;
			break;
		}
		GetDlgItem(IDC_ADCTInitStatus_LB)->SetWindowText("");
	}
		break;
	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CADCTDlg::OnDestroy()
{
	SetMainIniConfig();
	ADCTCopyDataSend(1, "ALL", 1, 99, "");
	ADCTCopyDataSend(3, "ALL", 1, 6, "");
	CDialogEx::OnDestroy();

}

void CADCTDlg::CheckStopCount(int PortIndex)
{
	if (CountMessageStopBit[PortIndex]==2&& PortUser[PortIndex]==1)
	{
		PortUser[PortIndex] = 0;
		GetDlgItem(StartBtn[PortIndex])->EnableWindow(true);
		GetDlgItem(PortStautsTB[PortIndex])->SetWindowText("停止");
		PortStatusPG[PortIndex].SetBarColor(RGB(255, 255, 255));
		ShowLog(PortIndex, "停止", 3);
	}
	else if(CountMessageStopBit[PortIndex] == 1&&PortUser[PortIndex]==3)
	{
		PortUser[PortIndex] = 0;
		GetDlgItem(StartBtn[PortIndex])->EnableWindow(true);
		GetDlgItem(PortStautsTB[PortIndex])->SetWindowText("停止");
		PortStatusPG[PortIndex].SetBarColor(RGB(255,255,255));
		ShowLog(PortIndex, "停止", 3);
	}
}
