// MetaDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "scs-client.h"
#include "MetaDlg.h"
#include "afxdialogex.h"


// CMetaDlg 对话框

IMPLEMENT_DYNAMIC(CMetaDlg, CDialogEx)

CMetaDlg::CMetaDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMetaDlg::IDD, pParent)
	, m_dlgGroupAcl(this)
	, m_dlgUserAcl(this)
	, m_strBucket(_T(""))
	, m_strObject(_T(""))
{

}

CMetaDlg::~CMetaDlg()
{
}

void CMetaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_META, m_listMeta);
}


BEGIN_MESSAGE_MAP(CMetaDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CMetaDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMetaDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CMetaDlg 消息处理程序


BOOL CMetaDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

//	m_listMeta.SetExtendedStyle(LVS_EX_GRIDLINES);
	m_listMeta.InsertColumn( 0, _T("Attribute"), LVCFMT_LEFT, 130);
	m_listMeta.InsertColumn( 1, _T("Value"), LVCFMT_LEFT, 240);

	m_dlgGroupAcl.Create(CAclListDlg::IDD, this);
	m_dlgUserAcl.Create(CAclListDlg::IDD, this);

	CRect rtList;
	CWnd* pGroup = GetDlgItem(IDC_STATIC_GROUP);
	ASSERT(pGroup != NULL);
	pGroup->GetWindowRect(rtList);
	this->ScreenToClient(rtList);
	m_dlgGroupAcl.SetWindowPos(NULL, rtList.left, rtList.bottom, 0, 0, 
		SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW);

	CWnd* pUser = GetDlgItem(IDC_STATIC_USER);
	ASSERT(pUser != NULL);
	pUser->GetWindowRect(rtList);
	this->ScreenToClient(rtList);
	m_dlgUserAcl.SetWindowPos(NULL, rtList.left, rtList.bottom, 0, 0, 
		SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CMetaDlg::AddAttribute( const CString& attr, const CString& val )
{
	int count = m_listMeta.GetItemCount();
	int id = m_listMeta.InsertItem(count, attr);
	m_listMeta.SetItemText(id, 1, val);
}

void CMetaDlg::SetCanonicalGroupAcl( char acl )
{
	m_dlgGroupAcl.AddItem(_T(CANONICAL_GROUP_SHOWNAME), acl);
}

void CMetaDlg::SetAnonymouseGroupAcl( char acl )
{
	m_dlgGroupAcl.AddItem(_T(ANONYMOUSE_GROUP_SHOWNAME), acl);
}

void CMetaDlg::AddUserAcl( const CString& name, char acl )
{
	m_dlgUserAcl.AddItem(name, acl);
}


void CMetaDlg::OnBnClickedOk()
{
	typedef std::map<CString, char> StrCharMap;

	BOOL bRemain = TRUE;
	ChangeAclParams params;
	params.buckname = m_strBucket;
	params.objname = m_strObject;

	char acl_c, acl_a;
	bRemain &= GetCanonicalGroupAcl(&acl_c);
	params.acls.insert(StrCharMap::value_type(_T(CANONICAL_GROUP), acl_c));

	bRemain &= GetAnonymouseGroupAcl(&acl_a);
	params.acls.insert(StrCharMap::value_type(_T(ANONYMOUSE_GROUP), acl_a));

	CList<CString, CString> users;
	GetAllUser(users);

	POSITION pos = users.GetHeadPosition();
	while (pos)
	{
		char acl;
		CString name = users.GetNext(pos);
		bRemain &= GetUserAcl(name, &acl);
		params.acls.insert(StrCharMap::value_type(name, acl));
	}

	if (!bRemain)
	{
		AfxGetApp()->GetMainWnd()->SendMessage(WM_MAINDLG_ChangeAcl, (WPARAM)&params);
	}

	CDialogEx::OnOK();
}

BOOL CMetaDlg::GetCanonicalGroupAcl( char* acl )
{
	return m_dlgGroupAcl.GetItem(_T(CANONICAL_GROUP_SHOWNAME), acl);
}

BOOL CMetaDlg::GetAnonymouseGroupAcl( char* acl )
{
	return m_dlgGroupAcl.GetItem(_T(ANONYMOUSE_GROUP_SHOWNAME), acl);
}

void CMetaDlg::GetAllUser( CList<CString, CString>& user )
{
	m_dlgUserAcl.GetAllUser(user);
}

BOOL CMetaDlg::GetUserAcl( const CString& name, char* acl )
{
	return m_dlgUserAcl.GetItem(name, acl);
}


void CMetaDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}

void CMetaDlg::SetBucketName( const CString& buckname )
{
	m_strBucket = buckname;
	SetWindowText(m_strBucket);
}

void CMetaDlg::SetObjcetName( const CString& objname )
{
	m_strObject = objname;
}

void CMetaDlg::ClearAll()
{
	m_strBucket = _T("");
	m_strObject = _T("");
	while (m_listMeta.DeleteItem(0)) {}
	m_dlgGroupAcl.DelAll();
	m_dlgUserAcl.DelAll();
}
