
// scs-clientDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "LoginDlg.h"
#include "TaskDef.h"
#include "OptDlg.h"
#include "TransListDlg.h"
#include "MetaDlg.h"
#include "FileInfoDlg.h"
#include "afxwin.h"
#include "BtnST.h"


// CscsclientDlg 对话框
class CscsclientDlg : public CDialogEx, public multi_http_msg_sender
{
// 构造
public:
	CscsclientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SCSCLIENT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnLogin(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCreateItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCurlMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChangeAcl(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnListServiceResult(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnListBucketResult(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCreateBucketResult(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCreateFolderResult(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeleteBucketResult(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeleteObjectResult(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUploadResult(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDownloadResult(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMetaResult(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAclResult(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetAclResult(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCopyResult(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRenameResult(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCancelTrans(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonRemove();
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnBnClickedButtonUpload();
	afx_msg void OnBnClickedButtonShowlog();
	afx_msg void OnBnClickedButtonDownload();
	afx_msg void OnBnClickedButtonRename();
	afx_msg void OnBusketlistMetainfo();
	afx_msg void OnFilelistInfo();
	afx_msg void OnLvnBeginlabeleditList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditList1(NMHDR *pNMHDR, LRESULT *pResult);

private:
	void Login();
	void BuildServerListHeader();
	void BuildBucketListHeader();
	void OrderServerListButtons();
	void OrderBucketListButtons();
	int GetIconIndex(LPCTSTR lpszPath, BOOL bDir);
	const CString& GetCurrentFolder();
	void ShowInfoMsg(const CString& msg);
	void ShowPathMsg();
	void ShowOptDlg(BOOL bShow);
	void OrderDlgItemPos();
	void ClearListContents();
	void ClearListColumns();
	void SaveConfigInfo(ConfigInfo& info);
	void LoadConfigInfo(ConfigInfo& info);

	void AddTask(TaskBase* task);
	void AddPriorTask(TaskBase* task);
	void AddUploadTask(UploadTask* task);
	void AddDownloadTask(DownloadTask* task);
	void RunNextTask();
	void RunNextUploadTask();
	void RunNextDownloadTask();
	void PopTask();
	void PopUploadTask(UINT64 id);
	void PopDownloadTask(UINT64 id);
	void CancelUploadTask(UINT64 id);
	void CancelDownloadTask(UINT64 id);
	void ShowUploadInfo();
	void ShowDownloadInfo();

	void ListService();
	void ListBucket(const CString& buckname, const CString& prefix = _T(""));
	void CreateBucket(const CString& buckname, S3CannedAcl acl);
	void CreateFolder(const CString& buckname, const CString& foldername, S3CannedAcl acl);
	void DeleteBucket(const CString& buckname);
	void DeleteObject(const CString& buckname, const CString& objname);
	void MetaBucket(const CString& buckname);
	void SetAcl(const CString& buckname, const CString& objname, std::map<CString, char>& acls);
	void GetAcl(const CString& buckname, const CString& objname, ListBucketItem* pItem = NULL);
	void Upload(const CString& buckname, const CString& filename,
		const CString& localpath, S3CannedAcl acl);
	void Download(const CString& buckname, const CString& filename,
		const CString& localpath);
	void Copy(const CString& buckname, const CString& frompath, const CString& topath);
	void Rename(const CString& buckname, const CString& frompath, const CString& topath);

	// asyn_msg_sender
public:
	virtual void post_message(unsigned long msg_id, unsigned long param1 = 0, unsigned long param2 = 0);

public:
	HICON m_hIcon;
	BOOL m_bShowBucket;
	CLoginDlg m_LoginDlg;
	COptDlg m_OptDlg;
	CTransListDlg m_dlgTransList;
	CMetaDlg m_dlgMeta;
	CFileInfoDlg m_dlgInfo;
	CListCtrl m_list;
	CString m_strBucket;
	CString	m_strDir;
	BOOL m_bExecuting;
	ConfigInfo m_cfgInfo;
	CString m_strConfigPath;

	CList<TaskBase*> m_Tasks;
	CList<TaskBase*> m_UploadTasks;
	CList<TaskBase*> m_DownloadTasks;
	BOOL m_bTaskRunning;
	
	int m_nEditItem;
	CButtonST m_btnTasks;
	CButtonST m_btnAdd;
	CButtonST m_btnRemove;
	CButtonST m_btnUpload;
	CButtonST m_btnDown;
	CButtonST m_btnRename;
	CButtonST m_btnRefresh;
};
