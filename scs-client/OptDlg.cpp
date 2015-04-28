// OptDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "scs-client.h"
#include "OptDlg.h"
#include "afxdialogex.h"


// COptDlg 对话框

IMPLEMENT_DYNAMIC(COptDlg, CDialogEx)

COptDlg::COptDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(COptDlg::IDD, pParent)
	, m_nCurrent(0)
{

}

COptDlg::~COptDlg()
{
}

void COptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}


BEGIN_MESSAGE_MAP(COptDlg, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// COptDlg 消息处理程序


BOOL COptDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);//选中整行 
	m_list.InsertColumn( 0, _T("Operation"), LVCFMT_LEFT, 120 );
	m_list.InsertColumn( 1, _T("State"), LVCFMT_LEFT, 100 );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void COptDlg::Add( TaskType type )
{
	CString opt;
	switch (type)
	{
	case kListService:
		opt = _T("ListServer");
		break;
	case kListBucket:
		opt = _T("ListBucket");
		break;
	case kDelectBucket:
		opt = _T("DelectBucket");
		break;
	case kDeleteObject:
		opt = _T("DeleteObject");
		break;
	case kCreateBucket:
		opt = _T("CreateBucket");
		break;
	case kCreateFolder:
		opt = _T("CreateFolder");
		break;
	case kUpload:
		opt = _T("Upload");
		break;
	case kDownload:
		opt = _T("Download");
		break;
	default:
		opt = _T("Unknown");
		break;
	}

	int count = m_list.GetItemCount();
	int id = m_list.InsertItem(count, opt);
	m_list.SetItemText(id, 1, _T("Running..."));
	m_list.EnsureVisible(id, FALSE); 
}

void COptDlg::Done( BOOL bDone )
{
	m_list.SetItemText(m_nCurrent, 1, _T("Done"));
	m_nCurrent ++;
}

void COptDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_list)
	{
		m_list.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE | SWP_SHOWWINDOW);
	}
}
