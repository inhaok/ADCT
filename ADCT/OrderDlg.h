#pragma once

#define DownloadSizeCheck_Timer 1001
// OrderDlg 对话框

class OrderDlg : public CDialogEx
{
	DECLARE_DYNAMIC(OrderDlg)

public:
	OrderDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~OrderDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_Order_Dialog };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg
		void SelectAllOrder();
	bool Download(CString IPAddress, CString Port, CString UserName, CString Password, CString fileName, CString serverFileName, int serverFileSize);
	void OnBnClickedReadoptionBtn();
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnEditchangeOrdernameCcb();
	void FuzzySearch(CStringArray & FuzzySearchInList, CString KeyWords, CStringArray & FuzzySearchOutList);
	void FindAllFiles(const char * lpPath);
	CComboBox m_OrderName_CCB;
protected:
	afx_msg LRESULT OnOrderdlgmessage(WPARAM wParam, LPARAM lParam);
	void DisposeDownloadFile();
	CProgressCtrl *m_Download_PG;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
};
