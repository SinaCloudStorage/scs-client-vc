#pragma once
#include "afxcmn.h"
#include "AclListDlg.h"


// CMetaDlg �Ի���

class CMetaDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMetaDlg)

public:
	CMetaDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMetaDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_META_INFO };

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	void ClearAll();

	void SetBucketName(const CString& buckname);
	void SetObjcetName(const CString& objname);

	void AddAttribute(const CString& attr, const CString& val);
	void SetCanonicalGroupAcl(char acl);
	void SetAnonymouseGroupAcl(char acl);
	void AddUserAcl(const CString& name, char acl);

	BOOL GetCanonicalGroupAcl(char* acl);
	BOOL GetAnonymouseGroupAcl(char* acl);

	void GetAllUser(CList<CString, CString>& user);
	BOOL GetUserAcl(const CString& name, char* acl);

	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	CListCtrl m_listMeta;
	CString m_strBucket;
	CString m_strObject;
	CAclListDlg	m_dlgGroupAcl;
	CAclListDlg	m_dlgUserAcl;
};
