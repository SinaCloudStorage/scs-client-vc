#pragma once
#include <afxtempl.h>

class CListItemDlg : public CDialog
{
//	DECLARE_DYNAMIC(CListItemDlg)

public:
	CListItemDlg(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	CListItemDlg(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual ~CListItemDlg();

public:
	void Add(CWnd* pItem);
	void Del(CWnd* pItem);
	void DelAll();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	void SetItemPos(CWnd * pItem);
	void MoveAllItems(int nTop);
	void SetVScrollStyle();
	void ChangeScrollInfo();
	/// 让最下面没有空隙
	void MakeBottomDown();

protected:
	CList<CWnd *>							m_ltItem;
	int										m_nHeight;
	int										m_nTop;
	BOOL									m_bVScroll;
	CBrush									m_brush;
};