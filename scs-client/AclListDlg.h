#pragma once
#include "ListItemDlg.h"

// CAclListDlg �Ի���

class CAclListDlg : public CListItemDlg
{
	DECLARE_DYNAMIC(CAclListDlg)

public:
	CAclListDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAclListDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_ACL_LIST };

	void AddItem(const CString& name, char acl);
	BOOL GetItem(const CString& name, char* acl);
	void GetAllUser(CList<CString, CString>& user);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
