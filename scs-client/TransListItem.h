#pragma once


// CTransListItem 对话框

class CTransListItem : public CDialogEx
{
	DECLARE_DYNAMIC(CTransListItem)

public:
	CTransListItem(DWORD dwID, CString strFilePath, BOOL bSend, DWORD dwFileSize, CWnd* pParent = NULL);
	virtual ~CTransListItem();

// 对话框数据
	enum { IDD = IDD_DIALOG_TRANS_ITEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedBtnCancel();

public:
	void ShowFileSize();
	void SetFileSize(DWORD dwSize);
	void SetTransSize(DWORD dwSize);
	void SetTransRate(DWORD dwRate);
	void SetNewFileId(DWORD dwID);

	BOOL BeSend() { return m_bSend; }
	DWORD GetFileId() { return m_dwID; }

	void SetTransing(BOOL bTrans);
	BOOL IsTransing() { return m_bTransing; }

private:
	CString GetSizeString(DWORD dwSize);
	CString GetRateString(DWORD dwRate);
	void SetProgress(DWORD dwTrans);

private:
	DWORD									m_dwID;

	CString									m_strFilePath;
	CString									m_strFileName;
	BOOL									m_bSend;
	BOOL									m_bTransing;

	/// size and rate, using Byte, show in KB
	DWORD									m_dwFileSize;
	DWORD									m_dwTransSize;

	CBrush									m_brush;

	CProgressCtrl							m_progress;
	
};
