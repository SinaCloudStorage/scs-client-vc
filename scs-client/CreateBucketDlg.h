#pragma once


// CCreateBucketDlg �Ի���

class CCreateBucketDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCreateBucketDlg)

public:
	CCreateBucketDlg(const CString& cap, const CString& lab, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCreateBucketDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_CREATE_BUCKET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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
