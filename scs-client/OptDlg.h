#pragma once
#include "afxcmn.h"
#include "TaskDef.h"

// COptDlg �Ի���

class COptDlg : public CDialogEx
{
	DECLARE_DYNAMIC(COptDlg)

public:
	COptDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~COptDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_OPERATION };

	afx_msg void OnSize(UINT nType, int cx, int cy);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	void Add(TaskType type);
	void Done(BOOL bDone);

public:
	virtual BOOL OnInitDialog();

public:
	CListCtrl m_list;
	UINT m_nCurrent;
};
