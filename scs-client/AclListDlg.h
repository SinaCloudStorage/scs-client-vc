#pragma once
#include "ListItemDlg.h"

// CAclListDlg 对话框

class CAclListDlg : public CListItemDlg
{
	DECLARE_DYNAMIC(CAclListDlg)

public:
	CAclListDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAclListDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_ACL_LIST };

	void AddItem(const CString& name, char acl);
	BOOL GetItem(const CString& name, char* acl);
	void GetAllUser(CList<CString, CString>& user);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
