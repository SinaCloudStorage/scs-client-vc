// TransListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "scs-client.h"
#include "TransListDlg.h"
#include "TransListItem.h"

#define SCROLL_PAGE_COUNT 100

// CTransListDlg dialog

IMPLEMENT_DYNAMIC(CTransListDlg, CListItemDlg)

CTransListDlg::CTransListDlg(CWnd* pParent /*=NULL*/)
	: CListItemDlg(CTransListDlg::IDD, pParent)
{
}

CTransListDlg::~CTransListDlg()
{
}

void CTransListDlg::DoDataExchange(CDataExchange* pDX)
{
	CListItemDlg::DoDataExchange(pDX);
}


// CTransListDlg message handlers

BOOL CTransListDlg::OnInitDialog()
{
	CListItemDlg::OnInitDialog();

	m_brush.CreateSolidBrush(RGB(255, 255, 255)); // 生成一白色刷子

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTransListDlg::DelItem(DWORD dwID, BOOL bSend)
{
	POSITION pos = m_ltItem.GetHeadPosition();

	while (pos)
	{
		POSITION posLast = pos;
		CTransListItem * pItem = (CTransListItem *)m_ltItem.GetNext(pos);

		if (pItem->BeSend() == bSend
			&& pItem->GetFileId() == dwID)
		{
			Del(pItem);
			break;
		}
	}
}

void CTransListDlg::AddItem(DWORD dwID, CString strFilePath, BOOL bSend, DWORD dwFileSize)
{
	CTransListItem * pItem = new CTransListItem(dwID, strFilePath, bSend, dwFileSize);
	pItem->Create(CTransListItem::IDD, this);
	Add(pItem);
}

void CTransListDlg::SetFileSize( DWORD dwID, BOOL bSend, DWORD dwSize )
{
	POSITION pos = m_ltItem.GetHeadPosition();

	while (pos)
	{
		CTransListItem * pItem = (CTransListItem *)m_ltItem.GetNext(pos);

		if (pItem->BeSend() == bSend
			&& pItem->GetFileId() == dwID)
		{
			pItem->SetFileSize(dwSize);
			break;
		}
	}
}

void CTransListDlg::SetTransing(DWORD dwID, BOOL bSend, BOOL bTrans)
{
	POSITION pos = m_ltItem.GetHeadPosition();

	while (pos)
	{
		CTransListItem * pItem = (CTransListItem *)m_ltItem.GetNext(pos);

		if (pItem->BeSend() == bSend
			&& pItem->GetFileId() == dwID)
		{
			pItem->SetTransing(bTrans);
			break;
		}
	}
}

void CTransListDlg::SetTransSize( DWORD dwID, BOOL bSend, DWORD dwSize )
{
	POSITION pos = m_ltItem.GetHeadPosition();

	while (pos)
	{
		CTransListItem * pItem = (CTransListItem *)m_ltItem.GetNext(pos);

		if (pItem->BeSend() == bSend
			&& pItem->GetFileId() == dwID)
		{
			pItem->SetTransSize(dwSize);
			break;
		}
	}
}

void CTransListDlg::SetTransRate( DWORD dwID, BOOL bSend, DWORD dwRate )
{
	POSITION pos = m_ltItem.GetHeadPosition();

	while (pos)
	{
		CTransListItem * pItem = (CTransListItem *)m_ltItem.GetNext(pos);

		if (pItem->BeSend() == bSend
			&& pItem->GetFileId() == dwID)
		{
			pItem->SetTransRate(dwRate);
			break;
		}
	}
}