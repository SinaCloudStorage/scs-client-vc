#pragma once
#include <afxtempl.h>
#include "ListItemDlg.h"


// CTransListDlg dialog

class CTransListDlg : public CListItemDlg
{
	DECLARE_DYNAMIC(CTransListDlg)

public:
	CTransListDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTransListDlg();

// Dialog Data
	enum { IDD = IDD_DLG_TRANS_LIST };

public:
	void AddItem(DWORD dwID, CString strFilePath, BOOL bSend, DWORD dwFileSize);

	void DelItem(DWORD dwID, BOOL bSend);

	void SetFileSize(DWORD dwID, BOOL bSend, DWORD dwSize);

	void SetTransing(DWORD dwID, BOOL bSend, BOOL bTrans);

	void SetTransSize(DWORD dwID, BOOL bSend, DWORD dwSize);

	void SetTransRate(DWORD dwID, BOOL bSend, DWORD dwRate);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();
};
