// TransListItem.cpp : 实现文件
//

#include "stdafx.h"
#include "scs-client.h"
#include "TransListItem.h"
#include "afxdialogex.h"

#define ICON_POS_X 2
#define ICON_POS_Y 2

// CTransListItem 对话框

IMPLEMENT_DYNAMIC(CTransListItem, CDialogEx)


CTransListItem::CTransListItem(DWORD dwID, CString strFilePath, BOOL bSend, DWORD dwFileSize, CWnd* pParent/* = NULL*/)
: CDialogEx(CTransListItem::IDD, pParent)
{
	m_bTransing = FALSE;
	m_dwTransSize = 0;

	SetNewFileId(dwID);

	m_dwFileSize = dwFileSize;

	m_strFilePath = strFilePath;
	int iPos = m_strFilePath.ReverseFind(_T('\\'));
	if (iPos == -1)
		m_strFileName = m_strFilePath;
	else
		m_strFileName = m_strFilePath.Right(m_strFilePath.GetLength() - iPos - 1);
	m_bSend = bSend;
}

CTransListItem::~CTransListItem()
{
}

void CTransListItem::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_TRANS, m_progress);
}


BEGIN_MESSAGE_MAP(CTransListItem, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &CTransListItem::OnBnClickedBtnCancel)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CTransListItem 消息处理程序
void CTransListItem::ShowFileSize()
{
	CString strFileSize = GetSizeString(m_dwFileSize);
	GetDlgItem(IDC_STATIC_SIZE)->SetWindowText(strFileSize);
}

void CTransListItem::SetFileSize(DWORD dwSize)
{
	m_dwFileSize = dwSize;
}

void CTransListItem::SetTransSize(DWORD dwSize)
{
	// 文件大小
	CString strFileSize = GetSizeString(m_dwFileSize);

	// 传输大小
	CString strTransSize  = GetSizeString(dwSize);

	CString str = strTransSize + _T("/") + strFileSize;

	GetDlgItem(IDC_STATIC_SIZE)->SetWindowText(str);
	GetDlgItem(IDC_STATIC_SIZE)->Invalidate();

	SetTransRate(dwSize - m_dwTransSize);
	m_dwTransSize = dwSize;

	SetProgress(dwSize);
}

void CTransListItem::SetTransRate(DWORD dwRate)
{
	CString strRate = GetRateString(dwRate);
	GetDlgItem(IDC_STATIC_RATE)->SetWindowText(strRate);
}

void CTransListItem::SetNewFileId(DWORD dwID)
{
	m_dwID = dwID;
}

CString CTransListItem::GetSizeString(DWORD dwSize)
{
	float fSizeK = dwSize / 1024;
	float fSizeM = fSizeK / 1024;

	CString strSize;
	if (fSizeM >= 1.0)
		strSize.Format(_T("%.1fM"), fSizeM);
	else
		strSize.Format(_T("%.0fK"), fSizeK);

	return strSize;
}

CString CTransListItem::GetRateString(DWORD dwRate)
{
	float fSizeK = dwRate / 1024;
	float fSizeM = fSizeK / 1024;

	CString strSize;
	if (fSizeM >= 1.0)
		strSize.Format(_T("%.1fM/S"), fSizeM);
	else if (fSizeK >= 1.0)
		strSize.Format(_T("%.0fK/S"), fSizeK);
	else
		strSize.Format(_T("%dB/S"), dwRate);

	return strSize;
}

void CTransListItem::SetProgress(DWORD dwTrans)
{
	DWORD dwPer;
	if (m_dwFileSize != 0)
	{
		if (m_dwFileSize > 40000000)
		{
			dwPer = dwTrans / (m_dwFileSize / 100);
		}
		else
		{
			dwPer = dwTrans * 100 / m_dwFileSize;
		}
	}
	else
	{
		dwPer = 0;
	}

	m_progress.SetPos(dwPer);
}

void CTransListItem::OnBnClickedBtnCancel()
{
	AfxGetApp()->GetMainWnd()->PostMessage(WM_MAINDLG_CancelTrans, (WPARAM)m_dwID, (LPARAM)m_bSend);
}

void CTransListItem::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// 绘制图标
	SHFILEINFO stFileInfo;
	if(::SHGetFileInfo(m_strFileName,FILE_ATTRIBUTE_NORMAL,     
		&stFileInfo,sizeof(stFileInfo),SHGFI_ICON | SHGFI_USEFILEATTRIBUTES))     
	{
		dc.DrawIcon(ICON_POS_X, ICON_POS_Y, stFileInfo.hIcon);  
	}
}


BOOL CTransListItem::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetDlgItem(IDC_STATIC_RATE)->SetWindowText(_T(""));

	ShowFileSize();

	m_brush.CreateSolidBrush(RGB(255, 255, 255)); // 生成一白色刷子

	SetTransing(FALSE);

	// 显示文件名
	CRect rtWnd;
	CWnd * pNameWnd = GetDlgItem(IDC_STATIC_FILE_NAME);
	CDC * pDC = pNameWnd->GetDC();
	pNameWnd->GetWindowRect(rtWnd);
	CSize size = pDC->GetTextExtent(m_strFileName);
	if (size.cx > rtWnd.Width())
	{
		int nCharWidth = size.cx / m_strFileName.GetLength();
		int nChar = rtWnd.Width() / nCharWidth;
		CString strShow = m_strFileName.Left(nChar - 3);
		strShow += _T("...");
		pNameWnd->SetWindowText(strShow);
	}
	else
	{
		GetDlgItem(IDC_STATIC_FILE_NAME)->SetWindowText(m_strFileName);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


HBRUSH CTransListItem::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
//	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return m_brush;
}

void CTransListItem::SetTransing( BOOL bTrans )
{
	m_bTransing = bTrans;

	if (m_bTransing)
	{
		if (m_bSend)
			GetDlgItem(IDC_STATIC_SEND_RECV)->SetWindowText(_T("Uploading"));
		else
			GetDlgItem(IDC_STATIC_SEND_RECV)->SetWindowText(_T("Downloading"));
	}
	else
	{
		if (m_bSend)
			GetDlgItem(IDC_STATIC_SEND_RECV)->SetWindowText(_T("Waitting for upload"));
		else
			GetDlgItem(IDC_STATIC_SEND_RECV)->SetWindowText(_T("Waitting for download"));
	}
}
