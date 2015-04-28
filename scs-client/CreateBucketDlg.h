#pragma once


// CCreateBucketDlg 对话框

class CCreateBucketDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCreateBucketDlg)

public:
	CCreateBucketDlg(const CString& cap, const CString& lab, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCreateBucketDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_CREATE_BUCKET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();

	void SetText(const CString& cap, const CString& lab);

public:
	CString m_strBucketName;
	CString m_strCaption;
	CString m_strLabel;
};
