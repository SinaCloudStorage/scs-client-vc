// CreateBucketDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "scs-client.h"
#include "CreateBucketDlg.h"
#include "afxdialogex.h"


// CCreateBucketDlg 对话框

IMPLEMENT_DYNAMIC(CCreateBucketDlg, CDialogEx)

CCreateBucketDlg::CCreateBucketDlg(const CString& cap, const CString& lab, CWnd* pParent)
	: CDialogEx(CCreateBucketDlg::IDD, pParent)
	, m_strCaption(cap)
	, m_strLabel(lab)
	, m_strBucketName(_T(""))
{

}

CCreateBucketDlg::~CCreateBucketDlg()
{
}

void CCreateBucketDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_BUCKET_NAME, m_strBucketName);
	DDV_MaxChars(pDX, m_strBucketName, 40);
	DDX_Text(pDX, IDC_STATIC_LABEL, m_strLabel);
}


BEGIN_MESSAGE_MAP(CCreateBucketDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CCreateBucketDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCreateBucketDlg 消息处理程序


void CCreateBucketDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	theApp.GetMainWnd()->SendMessage(WM_MAINDLG_CREATEITEM, (WPARAM)&m_strBucketName);
	CDialogEx::OnOK();
}


BOOL CCreateBucketDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(m_strCaption);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
