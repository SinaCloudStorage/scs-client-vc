#pragma once
#include "afxwin.h"


// CLoginDlg 对话框

class CLoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLoginDlg();

// 对话框数据
	enum { IDD = IDD_LOGIN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedButtonLogin();

public:
	HICON m_hIcon;
	CString m_strAccessKey;
	CString m_strAccessSecret;
	BOOL m_bUseHttps;
	BOOL m_bSaveSecret;
};
