#pragma once


// ManagerLoginDlg 对话框

class ManagerLoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ManagerLoginDlg)

public:
	ManagerLoginDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~ManagerLoginDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OptionLogin_Dlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
