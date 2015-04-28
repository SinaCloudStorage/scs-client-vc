#include "stdafx.h"
#include "ListItemDlg.h"


#define SCROLL_PAGE_COUNT 100

CListItemDlg::CListItemDlg( LPCTSTR lpszTemplateName, CWnd* pParentWnd /*= NULL*/ )
	: CDialog(lpszTemplateName, pParentWnd)
	, m_nHeight(0)
	, m_nTop(0)
	, m_bVScroll(FALSE)
{

}

CListItemDlg::CListItemDlg( UINT nIDTemplate, CWnd* pParentWnd /*= NULL*/ )
	: CDialog(nIDTemplate, pParentWnd)
	, m_nHeight(0)
	, m_nTop(0)
	, m_bVScroll(FALSE)
{

}

CListItemDlg::~CListItemDlg()
{
	POSITION pos = m_ltItem.GetHeadPosition();
	while (pos)
	{
		CWnd * pItem = m_ltItem.GetNext(pos);
		pItem->DestroyWindow();
		delete pItem;
		pItem = NULL;
	}
	m_ltItem.RemoveAll();
}

void CListItemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CListItemDlg, CDialog)
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

void CListItemDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLBARINFO info;
	GetScrollBarInfo(OBJID_CLIENT, &info);

	switch (nSBCode)
	{
	case SB_LINEDOWN:
		{
			int pos = GetScrollPos(SB_VERT) + 1;
			SetScrollPos(SB_VERT , pos);
			MoveAllItems(-pos);
		}
		break;

	case SB_LINEUP:
		{
			int pos = GetScrollPos(SB_VERT) - 1;
			SetScrollPos(SB_VERT , pos);
			MoveAllItems(-pos);
		}
		break;

	case SB_PAGEDOWN:
		{
			int pos = GetScrollPos(SB_VERT) + SCROLL_PAGE_COUNT;
			CRect rtClient;
			GetClientRect(rtClient);
			if (pos > m_nHeight - rtClient.Height())
				pos = m_nHeight - rtClient.Height();

			SetScrollPos(SB_VERT , pos);
			MoveAllItems(-pos);
		}
		break;

	case SB_PAGEUP:
		{
			int pos = GetScrollPos(SB_VERT) - SCROLL_PAGE_COUNT;
			if (pos < 0)
				pos = 0;

			SetScrollPos(SB_VERT , pos);
			MoveAllItems(-(int)pos);
		}
		break;

// 	case SB_THUMBPOSITION:
// 		{
// 			SetScrollPos(SB_VERT , nPos);
// 		}
// 		break;

	case SB_THUMBTRACK:
		{
			SetScrollPos(SB_VERT, nPos);
			MoveAllItems(-(int)nPos);
		}
		break;
	}

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}
void CListItemDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect rtClient;
	GetClientRect(rtClient);

	if (m_bVScroll)
	{
		ChangeScrollInfo();
	}

	// 让最下面没有空隙
	MakeBottomDown();

	// 由有滚动条变为没有滚动条
	if (rtClient.Height() >= m_nHeight && m_bVScroll)
	{
		MoveAllItems(0);
		m_bVScroll = FALSE;
	}

	// 由没有滚动条变为有滚动条
	if (rtClient.Height() < m_nHeight && !m_bVScroll)
	{
		ChangeScrollInfo();

		MoveAllItems(0);
		m_bVScroll = TRUE;
	}
}

HBRUSH CListItemDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if(nCtlColor == CTLCOLOR_DLG)
		return m_brush;

	return hbr;
}

void CListItemDlg::SetItemPos(CWnd * pItem)
{
	CRect rtClient, rtItem;
	GetClientRect(rtClient);
	pItem->GetWindowRect(rtItem);

	pItem->SetWindowPos(NULL, 
		(rtClient.Width() - rtItem.Width()) / 2,
		m_nTop + m_nHeight,
		0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);

	m_nHeight += rtItem.Height();
}

void CListItemDlg::MoveAllItems(int nTop)
{
	m_nTop = nTop;
	m_nHeight = 0;

	POSITION pos = m_ltItem.GetHeadPosition();
	while (pos)
	{
		CWnd * pItem = m_ltItem.GetNext(pos);
		SetItemPos(pItem);
	}
}

void CListItemDlg::SetVScrollStyle()
{
	LONG lStyle = GetWindowLong(GetSafeHwnd(), GWL_STYLE);
	LONG lStyleOld = lStyle;
	lStyle |= WS_VSCROLL;
	SetWindowLong(GetSafeHwnd(), GWL_STYLE, lStyle);
	SetWindowPos(NULL,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

void CListItemDlg::ChangeScrollInfo()
{
	CRect rtClient;
	GetClientRect(rtClient);

	SCROLLINFO info;
	info.cbSize = sizeof(SCROLLINFO);
	info.nMin = 0;
	info.nMax = m_nHeight - rtClient.Height() + SCROLL_PAGE_COUNT;
	info.nPage = SCROLL_PAGE_COUNT;
	info.fMask = SIF_RANGE | SIF_PAGE;
	SetScrollInfo(SB_VERT, &info);
}

void CListItemDlg::MakeBottomDown()
{
	CRect rtClient;
	GetClientRect(rtClient);

	if (m_nTop < 0 && m_nTop + m_nHeight < rtClient.Height())
	{
		m_nTop = rtClient.Height() - m_nHeight;
		if (m_nTop > 0)
			m_nTop = 0;

		MoveAllItems(m_nTop);
	}
}

void CListItemDlg::Add( CWnd* pItem )
{
	// set position
	SetItemPos(pItem);

	m_ltItem.AddTail(pItem);

	// add v-scroll
	CRect rtClient;
	GetClientRect(rtClient);
	if (m_nHeight > rtClient.Height() && !m_bVScroll)
	{
		SetVScrollStyle();

		MoveAllItems(m_nTop);
		m_bVScroll = TRUE;
	}

	if (m_bVScroll)
	{
		ChangeScrollInfo();
	}
}

void CListItemDlg::Del( CWnd* pItem )
{
	POSITION pos = m_ltItem.GetHeadPosition();
	while (pos)
	{
		POSITION posLast = pos;
		CWnd * pWnd = m_ltItem.GetNext(pos);

		if (pWnd == pItem)
		{
			m_ltItem.RemoveAt(posLast);
			MoveAllItems(m_nTop);
			ChangeScrollInfo();
			MakeBottomDown();
			break;
		}
	}
	pItem->DestroyWindow();
	delete pItem;
	pItem = NULL;
}

void CListItemDlg::DelAll()
{
	while (!m_ltItem.IsEmpty())
	{
		Del(m_ltItem.GetHead());
	}
}
