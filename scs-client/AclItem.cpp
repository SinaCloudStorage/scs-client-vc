// AclItem.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "scs-client.h"
#include "AclItem.h"
#include "afxdialogex.h"


// CAclItem �Ի���

IMPLEMENT_DYNAMIC(CAclItem, CDialogEx)

CAclItem::CAclItem(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAclItem::IDD, pParent)
	, m_strName(_T(""))
	, m_nAcl(0)
{

}

CAclItem::~CAclItem()
{
}

void CAclItem::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_NAME, m_strName);
	DDX_Check(pDX, IDC_CHECK_READ, m_bAcls[0]);
	DDX_Check(pDX, IDC_CHECK_WRITE, m_bAcls[1]);
	DDX_Check(pDX, IDC_CHECK_READACP, m_bAcls[2]);
	DDX_Check(pDX, IDC_CHECK_WRITEACP, m_bAcls[3]);
}


BEGIN_MESSAGE_MAP(CAclItem, CDialogEx)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CAclItem ��Ϣ�������


HBRUSH CAclItem::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
//	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return m_brush;
}


BOOL CAclItem::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_brush.CreateSolidBrush(RGB(255, 255, 255)); // ����һ��ɫˢ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CAclItem::SetName( const CString& name )
{
	m_strName = name;
	UpdateData(FALSE);
}

const CString& CAclItem::GetName()
{
	return m_strName;
}

void CAclItem::SetAcl( char acl )
{
	m_nAcl = acl;
	for (int i = 0; i < ACL_COUNT; ++ i)
	{
		m_bAcls[i] = acl & 0x1;
		acl >>= 1;
	}
	UpdateData(FALSE);
}

BOOL CAclItem::GetAcl(char* acl)
{
	*acl = 0;
	UpdateData(TRUE);
	for (int i = 0; i < ACL_COUNT; ++ i)
	{
		*acl |= ((m_bAcls[i] ? 1 : 0) << i);
	}
	return m_nAcl == *acl;
}
