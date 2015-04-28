// LoginDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "scs-client.h"
#include "LoginDlg.h"
#include "afxdialogex.h"


// CLoginDlg �Ի���

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoginDlg::IDD, pParent)
	, m_strAccessKey(_T(""))
	, m_strAccessSecret(_T(""))
	, m_bUseHttps(FALSE)
	, m_bSaveSecret(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ACCESS_KEY, m_strAccessKey);
	DDV_MaxChars(pDX, m_strAccessKey, 128);
	DDX_Text(pDX, IDC_EDIT_ACCESS_SECRET, m_strAccessSecret);
	DDV_MaxChars(pDX, m_strAccessSecret, 128);
	DDX_Check(pDX, IDC_CHECK_USE_HTTPS, m_bUseHttps);
	DDX_Check(pDX, IDC_CHECK_SAVE_SECRET, m_bSaveSecret);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CLoginDlg::OnBnClickedButtonLogin)
END_MESSAGE_MAP()


// CLoginDlg ��Ϣ�������


void CLoginDlg::OnBnClickedButtonLogin()
{
	UpdateData(TRUE);
	LoginParams* params = new LoginParams;
	params->strAccessKey = m_strAccessKey;
	params->strAccessSecret = m_strAccessSecret;
	params->bUseHttps = m_bUseHttps;
	params->bSaveSecret = m_bSaveSecret;
	theApp.GetMainWnd()->PostMessage(WM_MAINDLG_LOGIN, (WPARAM)params);
}


BOOL CLoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��


	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
