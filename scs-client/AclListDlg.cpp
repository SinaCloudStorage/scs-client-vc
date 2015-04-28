// AclListDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "scs-client.h"
#include "AclListDlg.h"
#include "AclItem.h"


// CAclListDlg �Ի���

IMPLEMENT_DYNAMIC(CAclListDlg, CListItemDlg)

CAclListDlg::CAclListDlg(CWnd* pParent /*=NULL*/)
	: CListItemDlg(CAclListDlg::IDD, pParent)
{

}

CAclListDlg::~CAclListDlg()
{
}

void CAclListDlg::DoDataExchange(CDataExchange* pDX)
{
	CListItemDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAclListDlg, CListItemDlg)
END_MESSAGE_MAP()

void CAclListDlg::AddItem( const CString& name, char acl )
{
	CAclItem * pItem = new CAclItem;
	pItem->Create(CAclItem::IDD, this);
	pItem->SetName(name);
	pItem->SetAcl(acl);
	Add(pItem);
}


// CAclListDlg ��Ϣ�������


BOOL CAclListDlg::OnInitDialog()
{
	CListItemDlg::OnInitDialog();

	m_brush.CreateSolidBrush(RGB(255, 255, 255)); // ����һ��ɫˢ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE

}

BOOL CAclListDlg::GetItem( const CString& name, char* acl )
{
	POSITION pos = m_ltItem.GetHeadPosition();
	while (pos)
	{
		CAclItem * pItem = (CAclItem *)m_ltItem.GetNext(pos);

		if (pItem->GetName() == name)
		{
			return pItem->GetAcl(acl);
		}
	}
	ASSERT(FALSE);
	*acl = 0;
	return TRUE;
}

void CAclListDlg::GetAllUser( CList<CString, CString>& user )
{
	POSITION pos = m_ltItem.GetHeadPosition();
	while (pos)
	{
		CAclItem * pItem = (CAclItem *)m_ltItem.GetNext(pos);
		user.AddTail(pItem->GetName());
	}
}
