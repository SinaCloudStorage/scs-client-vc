// AclListDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "scs-client.h"
#include "AclListDlg.h"
#include "AclItem.h"


// CAclListDlg 对话框

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


// CAclListDlg 消息处理程序


BOOL CAclListDlg::OnInitDialog()
{
	CListItemDlg::OnInitDialog();

	m_brush.CreateSolidBrush(RGB(255, 255, 255)); // 生成一白色刷子

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE

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
