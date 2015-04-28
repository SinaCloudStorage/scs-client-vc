#pragma once


// CAclItem �Ի���

class CAclItem : public CDialogEx
{
	DECLARE_DYNAMIC(CAclItem)

public:
	CAclItem(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAclItem();

// �Ի�������
	enum { IDD = IDD_DIALOG_ACL_ITEM };

	void SetName(const CString& name);
	const CString& GetName();

	void SetAcl(char acl);
	BOOL GetAcl(char* acl);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	CBrush									m_brush;
	
	CString									m_strName;
	char									m_nAcl;
	BOOL									m_bAcls[ACL_COUNT];
};
