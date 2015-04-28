// CFileInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "scs-client.h"
#include "FileInfoDlg.h"
#include "afxdialogex.h"


// CFileInfoDlg 对话框

IMPLEMENT_DYNAMIC(CFileInfoDlg, CDialogEx)

CFileInfoDlg::CFileInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFileInfoDlg::IDD, pParent)
	, m_dlgGroupAcl(this)
	, m_dlgUserAcl(this)
	, m_strBucket(_T(""))
	, m_strObject(_T(""))
{

}

CFileInfoDlg::~CFileInfoDlg()
{
}

void CFileInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_INFO, m_listInfo);
}


BEGIN_MESSAGE_MAP(CFileInfoDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CFileInfoDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CFileInfoDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

void CFileInfoDlg::ClearAll()
{
	m_strBucket = _T("");
	m_strObject = _T("");
	while (m_listInfo.DeleteItem(0)) {}
	m_dlgGroupAcl.DelAll();
	m_dlgUserAcl.DelAll();
}

void CFileInfoDlg::SetBucketName( const CString& buckname )
{
	m_strBucket = buckname;
}

void CFileInfoDlg::SetObjcetName( const CString& objname )
{
	m_strObject = objname;
	SetWindowText(m_strObject);
}

void CFileInfoDlg::AddAttribute( const CString& attr, const CString& val )
{
	int count = m_listInfo.GetItemCount();
	int id = m_listInfo.InsertItem(count, attr);
	m_listInfo.SetItemText(id, 1, val);
}

void CFileInfoDlg::SetCanonicalGroupAcl( char acl )
{
	m_dlgGroupAcl.AddItem(_T(CANONICAL_GROUP_SHOWNAME), acl);
}

void CFileInfoDlg::SetAnonymouseGroupAcl( char acl )
{
	m_dlgGroupAcl.AddItem(_T(ANONYMOUSE_GROUP_SHOWNAME), acl);
}

void CFileInfoDlg::AddUserAcl( const CString& name, char acl )
{
	m_dlgUserAcl.AddItem(name, acl);
}

BOOL CFileInfoDlg::GetCanonicalGroupAcl( char* acl )
{
	return m_dlgGroupAcl.GetItem(_T(CANONICAL_GROUP_SHOWNAME), acl);
}

BOOL CFileInfoDlg::GetAnonymouseGroupAcl( char* acl )
{
	return m_dlgGroupAcl.GetItem(_T(ANONYMOUSE_GROUP_SHOWNAME), acl);
}

void CFileInfoDlg::GetAllUser( CList<CString, CString>& user )
{
	m_dlgUserAcl.GetAllUser(user);
}

BOOL CFileInfoDlg::GetUserAcl( const CString& name, char* acl )
{
	return m_dlgUserAcl.GetItem(name, acl);
}


// CFileInfoDlg 消息处理程序


void CFileInfoDlg::OnBnClickedOk()
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


void CFileInfoDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


BOOL CFileInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_listInfo.InsertColumn( 0, _T("Attribute"), LVCFMT_LEFT, 130);
	m_listInfo.InsertColumn( 1, _T("Value"), LVCFMT_LEFT, 240);

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
