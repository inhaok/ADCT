#pragma once


// DLOption 对话框

extern CString DAFileBackup;

class DLOption : public CDialogEx
{
	DECLARE_DYNAMIC(DLOption)

public:
	DLOption(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~DLOption();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLOption };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg LRESULT OnDloptionsmessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CString DownloadFilePathBackup;
	CEdit m_DAFileName_TB;
	CComboBox m_BaudCCB;
	CComboBox m_DownloadTypeCCB;
	afx_msg void OnMenuClick(UINT nID);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
	void CCBInit();
	int COMPortType;
	afx_msg void OnBnClickedUartRd();
	int DownloadFormatOperation;
	afx_msg void OnBnClickedAutoRd();
	int FlashType;
	CString m_Baud;
	CString m_DownloadType;
	afx_msg void OnBnClickedNandRd();
	void DLTypeOption();
	void SetPortCCB();
	CComboBox m_PowerComCCB;
	CString m_RadioAddressText;
	CString m_PowerComText;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CString m_SleepTimer;
	CStringArray MenuCheckBackup;
	
protected:
//	afx_msg LRESULT OnDloption(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDloption(WPARAM wParam, LPARAM lParam);
};
