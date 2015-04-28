
// scs-clientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "scs-client.h"
#include "scs-clientDlg.h"
#include "afxdialogex.h"
#include "CreateBucketDlg.h"
#include "util.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ID_RunTaskTimeout		1100
#define ID_ShowTaskTimeout		1101
#define ID_ShowMsgTimeout		1102
#define TRANS_COLOR_RGB			RGB(240, 240, 240)

UINT64 TaskBase::_maxid = 0;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CscsclientDlg 对话框



CscsclientDlg::CscsclientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CscsclientDlg::IDD, pParent)
	, m_OptDlg(this)
	, m_dlgTransList(this)
	, m_strBucket(_T(""))
	, m_strDir(_T(""))
	, m_bExecuting(FALSE)
	, m_bTaskRunning(FALSE)
	, m_nEditItem(0)
	, m_strConfigPath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CscsclientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_CHECK_SHOW_TRANS, m_btnTasks);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_btnAdd);
	DDX_Control(pDX, IDC_BUTTON_REMOVE, m_btnRemove);
	DDX_Control(pDX, IDC_BUTTON_UPLOAD, m_btnUpload);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD, m_btnDown);
	DDX_Control(pDX, IDC_BUTTON_RENAME, m_btnRename);
	DDX_Control(pDX, IDC_BUTTON_REFRESH, m_btnRefresh);
}

BEGIN_MESSAGE_MAP(CscsclientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_MAINDLG_LOGIN, OnLogin)
	ON_MESSAGE(WM_MAINDLG_CREATEITEM, OnCreateItem)
	ON_MESSAGE(WM_MAINDLG_MULITCURL, OnCurlMsg)
	ON_MESSAGE(WM_MAINDLG_ChangeAcl, OnChangeAcl)
	ON_MESSAGE(WM_MAINDLG_ListServiceResult, OnListServiceResult)
	ON_MESSAGE(WM_MAINDLG_ListBucketResult, OnListBucketResult)
	ON_MESSAGE(WM_MAINDLG_CreateBucketResult, OnCreateBucketResult)
	ON_MESSAGE(WM_MAINDLG_CreateFolderResult, OnCreateFolderResult)
	ON_MESSAGE(WM_MAINDLG_DeleteBucketResult, OnDeleteBucketResult)
	ON_MESSAGE(WM_MAINDLG_DeleteObjectResult, OnDeleteObjectResult)
	ON_MESSAGE(WM_MAINDLG_UploadResult, OnUploadResult)
	ON_MESSAGE(WM_MAINDLG_DownloadResult, OnDownloadResult)
	ON_MESSAGE(WM_MAINDLG_MetaResult, OnMetaResult)
	ON_MESSAGE(WM_MAINDLG_AclResult, OnAclResult)
	ON_MESSAGE(WM_MAINDLG_GetAclResult, OnGetAclResult)
	ON_MESSAGE(WM_MAINDLG_CopyResult, OnCopyResult)
	ON_MESSAGE(WM_MAINDLG_RenameResult, OnRenameResult)
	ON_MESSAGE(WM_MAINDLG_CancelTrans, OnCancelTrans)

	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CscsclientDlg::OnNMDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CscsclientDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, &CscsclientDlg::OnBnClickedButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CscsclientDlg::OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD, &CscsclientDlg::OnBnClickedButtonUpload)
	ON_BN_CLICKED(IDC_CHECK_SHOW_TRANS, &CscsclientDlg::OnBnClickedButtonShowlog)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD, &CscsclientDlg::OnBnClickedButtonDownload)
	ON_BN_CLICKED(IDC_BUTTON_RENAME, &CscsclientDlg::OnBnClickedButtonRename)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CscsclientDlg::OnNMRClickList1)
	ON_WM_TIMER()
	ON_COMMAND(ID_BUSKETLIST_METAINFO, &CscsclientDlg::OnBusketlistMetainfo)
	ON_COMMAND(ID_BUSKETLIST_DELETE, &CscsclientDlg::OnBnClickedButtonRemove)
	ON_COMMAND(ID_FILELIST_INFO, &CscsclientDlg::OnFilelistInfo)
	ON_COMMAND(ID_FILELIST_RENAME, &CscsclientDlg::OnBnClickedButtonRename)
	ON_COMMAND(ID_FILELIST_DELETE, &CscsclientDlg::OnBnClickedButtonRemove)
	ON_COMMAND(ID_FILELIST_DOWNLOAD, &CscsclientDlg::OnBnClickedButtonDownload)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_LIST1, &CscsclientDlg::OnLvnBeginlabeleditList1)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST1, &CscsclientDlg::OnLvnEndlabeleditList1)
END_MESSAGE_MAP()


// CscsclientDlg 消息处理程序

BOOL CscsclientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// 为CListCtrl添加系统图标
	HIMAGELIST    himlSmall;  
	SHFILEINFO    sfi;  
	TCHAR        szDir[MAX_PATH] = { 0 }; 
	GetWindowsDirectory(szDir,   MAX_PATH);  
	himlSmall = (HIMAGELIST)SHGetFileInfo(
		(LPCTSTR)szDir,
		0,
		&sfi,
		sizeof(SHFILEINFO),
		SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	if (himlSmall)
	{
		::SendMessage(m_list.m_hWnd, LVM_SETIMAGELIST, (WPARAM)LVSIL_SMALL, (LPARAM)himlSmall);
	}
	
	m_btnAdd.DrawBorder(FALSE);
	m_btnAdd.SetBitmaps(IDB_BITMAP_ADD_C, TRANS_COLOR_RGB, IDB_BITMAP_ADD, TRANS_COLOR_RGB);
	m_btnAdd.SetAlign(CButtonST::ST_ALIGN_VERT);

	m_btnRemove.DrawBorder(FALSE);
	m_btnRemove.SetBitmaps(IDB_BITMAP_REMOVE_C, TRANS_COLOR_RGB, IDB_BITMAP_REMOVE, TRANS_COLOR_RGB);
	m_btnRemove.SetAlign(CButtonST::ST_ALIGN_VERT);

	m_btnUpload.DrawBorder(FALSE);
	m_btnUpload.SetBitmaps(IDB_BITMAP_UPLOAD_C, TRANS_COLOR_RGB, IDB_BITMAP_UPLOAD, TRANS_COLOR_RGB);
	m_btnUpload.SetAlign(CButtonST::ST_ALIGN_VERT);

	m_btnDown.DrawBorder(FALSE);
	m_btnDown.SetBitmaps(IDB_BITMAP_DOWN_C, TRANS_COLOR_RGB, IDB_BITMAP_DOWN, TRANS_COLOR_RGB);
	m_btnDown.SetAlign(CButtonST::ST_ALIGN_VERT);

	m_btnRename.DrawBorder(FALSE);
	m_btnRename.SetBitmaps(IDB_BITMAP_RENAME_C, TRANS_COLOR_RGB, IDB_BITMAP_RENAME, TRANS_COLOR_RGB);
	m_btnRename.SetAlign(CButtonST::ST_ALIGN_VERT);

	m_btnRefresh.DrawBorder(FALSE);
	m_btnRefresh.SetBitmaps(IDB_BITMAP_REFRESH_C, TRANS_COLOR_RGB, IDB_BITMAP_REFRESH, TRANS_COLOR_RGB);
	m_btnRefresh.SetAlign(CButtonST::ST_ALIGN_VERT);

	m_btnTasks.DrawBorder(FALSE);
	m_btnTasks.SetBitmaps(IDB_BITMAP_LIST_C, TRANS_COLOR_RGB, IDB_BITMAP_LIST, TRANS_COLOR_RGB);
	m_btnTasks.SetAlign(CButtonST::ST_ALIGN_VERT);

	// TODO: 在此添加额外的初始化代码//LVS_EDITLABELS
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);//选中整行 
	m_dlgTransList.Create(CTransListDlg::IDD, this);
	m_dlgTransList.SetParent(this);
	m_OptDlg.Create(IDD_DIALOG_OPERATION);
	m_bShowBucket = FALSE;
	singleton_ex<multi_http>::get_instance()->set_message_sender(this);
	Login();
	
	this->SetTimer(ID_RunTaskTimeout, 500, NULL);
	this->SetTimer(ID_ShowTaskTimeout, 1000, NULL);
	this->SetTimer(ID_ShowMsgTimeout, 2000, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CscsclientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CscsclientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CscsclientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CscsclientDlg::Login()
{
// 	CString strKey = _T("t9rflt1sgvJVDTQtikQ3");
// 	CString strSecret = _T("25cf5ec8fce83e9402eeac201f93ff11482b6808");

	// get saved access key & secret
	CString strAppPath;
	if (!GetAppDataPath(strAppPath) ||
		!DirectoryExists(strAppPath))
	{
		GetExePath(strAppPath);
	}
	m_strConfigPath = strAppPath;
	LoadConfigInfo(m_cfgInfo);

	if (m_cfgInfo.bSaveSecret)
	{
		m_LoginDlg.m_strAccessKey = m_cfgInfo.strAccessKey;
		m_LoginDlg.m_strAccessSecret = m_cfgInfo.strAccessSecret;
	}
	
	m_LoginDlg.m_bUseHttps = m_cfgInfo.bUseHttps;
	m_LoginDlg.m_bSaveSecret = m_cfgInfo.bSaveSecret;
	if (m_LoginDlg.DoModal() != IDOK)
	{
		PostQuitMessage(0);
	}
}


LRESULT CscsclientDlg::OnLogin( WPARAM wParam, LPARAM lParam )
{
	LoginParams* params = (LoginParams*)wParam;
	TaskBase::_s_access_key = CString2string(params->strAccessKey);
	TaskBase::_s_secret_key = CString2string(params->strAccessSecret);

	if (params->bUseHttps != m_cfgInfo.bUseHttps)
	{
		m_cfgInfo.bUseHttps = params->bUseHttps;
		SaveConfigInfo(m_cfgInfo);
	}
	delete params;
	ListService();
	return S_OK;
}


LRESULT CscsclientDlg::OnCreateItem( WPARAM wParam, LPARAM lParam )
{
	CString* pstrName = (CString*)wParam;
	if (m_bShowBucket)
	{
		CreateBucket(*pstrName, S3CannedAclPrivate);
	}
	else
	{
		CString dirname = GetCurrentFolder() + *pstrName + _T("/");
		CreateFolder(m_strBucket, dirname, S3CannedAclPrivate);
	}
	return S_OK;
}

LRESULT CscsclientDlg::OnCurlMsg( WPARAM wParam, LPARAM lParam )
{
	CurlMsgParams* params = (CurlMsgParams*)wParam;
	multi_http::handle_message(params->msg_id, params->param1, params->param2);
	delete params;
	return S_OK;
}

LRESULT CscsclientDlg::OnChangeAcl( WPARAM wParam, LPARAM lParam )
{
	ChangeAclParams* params = (ChangeAclParams*)wParam;
	SetAcl(params->buckname, params->objname, params->acls);
	delete params;
	return S_OK;
}

LRESULT CscsclientDlg::OnListServiceResult( WPARAM wParam, LPARAM lParam )
{
	if (wParam == 0)
	{
		ListServiceResult* result = (ListServiceResult*)lParam;
		if (result->status == 200)
		{
			BuildServerListHeader();
			std::vector<ListServiceItem>::iterator it = result->items.begin();
			for (; it != result->items.end(); ++ it)
			{
				int count = m_list.GetItemCount();
				int id = m_list.InsertItem(count, it->name);
				m_list.SetItemText(id, 1, it->date);
			}
			if (::IsWindow(m_LoginDlg.GetSafeHwnd()))
			{
				m_cfgInfo.bSaveSecret = m_LoginDlg.m_bSaveSecret;
				m_cfgInfo.bUseHttps = m_LoginDlg.m_bUseHttps;
				m_cfgInfo.strAccessKey = m_LoginDlg.m_strAccessKey;
				if (m_cfgInfo.bSaveSecret)
				{
					m_cfgInfo.strAccessSecret = m_LoginDlg.m_strAccessSecret;
				}
				else
				{
					m_cfgInfo.strAccessSecret = _T("");
				}
				
				SaveConfigInfo(m_cfgInfo);

				m_LoginDlg.EndDialog(IDOK);
			}
			ShowPathMsg();
		}
		else
		{
			// error
		}
		delete result;
	}
	else
	{
		// curl error
	}
	
	m_strBucket = _T("");
	m_strDir = _T("");
	PopTask();
	RunNextTask();
	return S_OK;
}

void CscsclientDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	CString name = m_list.GetItemText(pNMItemActivate->iItem, 0);
	if (!name.IsEmpty())
	{
		if (m_bShowBucket)
		{
			ListBucket(name);
		}
		else
		{
			CString prefixName = _T("");
			if (pNMItemActivate->iItem == 0 &&
				GetCurrentFolder().IsEmpty())
			{
				ListService();
			}
			else
			{
				if (pNMItemActivate->iItem == 0)
				{
					CString str = GetCurrentFolder();
					str.Delete(str.GetLength() - 1);
					int pos = str.ReverseFind(_T('/'));
					if (pos != -1)
					{
						prefixName = str.Left(pos+1);
					}
				}
				else
				{
					prefixName = GetCurrentFolder() + name;
				}
				ListBucket(m_strBucket, prefixName);
			}
		}
	}

	*pResult = 0;
}

void CscsclientDlg::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	CString name = m_list.GetItemText(pNMItemActivate->iItem, 0);
	if (!name.IsEmpty() && name != _T(".."))
	{
		CMenu menu;
		int pos = m_bShowBucket ? 1 : 0;
		menu.LoadMenu(IDR_MENU_FILELIST);
		CMenu *pMenu = menu.GetSubMenu(pos);
		if (!m_bShowBucket &&
			name.ReverseFind('/') != -1)
		{
			pMenu->EnableMenuItem(ID_FILELIST_DOWNLOAD, MF_GRAYED);
			pMenu->EnableMenuItem(ID_FILELIST_RENAME, MF_GRAYED);
		}
		CPoint pt;
		GetCursorPos(&pt); 
		pMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
	}

	*pResult = 0;
}

LRESULT CscsclientDlg::OnListBucketResult( WPARAM wParam, LPARAM lParam )
{
	if (wParam == 0)
	{
		ListBucketResult* result = (ListBucketResult*)lParam;
		ASSERT(result);

		if (result->status == 200)
		{
			ClearListContents();
			BuildBucketListHeader();

			int iIcon = GetIconIndex(_T("folder"), TRUE);
			int count = m_list.GetItemCount();
			int id = m_list.InsertItem(count, _T(".."), iIcon);
			m_list.SetItemText(id, 1, _T("--"));
			m_list.SetItemText(id, 2, _T("--"));
			m_list.SetItemText(id, 3, _T("--"));

			std::vector<ListBucketItem>::iterator it = result->items.begin();
			for (; it != result->items.end(); ++ it)
			{
				CString name = it->name;
				ASSERT(name.Left(result->foldername.GetLength()) == result->foldername);
				name = name.Right(name.GetLength() - result->foldername.GetLength());
				if (!name.IsEmpty())
				{
					int iIcon = GetIconIndex(name, it->is_dir);
					int count = m_list.GetItemCount();
					int id = m_list.InsertItem(count, name, iIcon);
					m_list.SetItemText(id, 1, it->is_dir ? _T("--") : format_size(it->size));
					m_list.SetItemText(id, 2, it->is_dir ? _T("--") : it->date);
					m_list.SetItemText(id, 3, it->is_dir ? _T("--") : it->md5);
				}
			}
			m_strBucket = result->buckname;
			m_strDir = result->foldername;
			ShowPathMsg();
		}
		else
		{
			// error
		}
		delete result;
	}
	else
	{
		// curl error
	}
	PopTask();
	RunNextTask();
	return S_OK;
}

void CscsclientDlg::BuildServerListHeader()
{
	ClearListContents();
	if (!m_bShowBucket)
	{
		OrderServerListButtons();

		ClearListColumns();
		m_list.InsertColumn( 0, _T("Bucket Name"), LVCFMT_LEFT, BucketNameColumnWidth );
		m_list.InsertColumn( 1, _T("Create Time"), LVCFMT_LEFT, BucketTimeColumnWidth );
		m_bShowBucket = TRUE;
	}
}

void CscsclientDlg::BuildBucketListHeader()
{
	ClearListContents();
	if (m_bShowBucket)
	{
		OrderBucketListButtons();

		ClearListColumns();
		m_list.InsertColumn( 0, _T("Name"),			LVCFMT_LEFT,	FileNameColumnWidth );
		m_list.InsertColumn( 1, _T("Size"),			LVCFMT_RIGHT,	FileSizeColumnWidth );
		m_list.InsertColumn( 2, _T("Modifed Time"), LVCFMT_LEFT,	ModifedTimeColumnWidth );
		m_list.InsertColumn( 3, _T("MD5"),			LVCFMT_LEFT,	FileMd5ColumnWidth );
		m_bShowBucket = FALSE;
	}
}

int CscsclientDlg::GetIconIndex( LPCTSTR lpszPath, BOOL bDir )
{
	SHFILEINFO sfi;
	memset(&sfi, 0, sizeof(sfi));
	DWORD dwFileAttributes = bDir ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;

	SHGetFileInfo(lpszPath,
		dwFileAttributes,
		&sfi,
		sizeof(sfi),
		SHGFI_SMALLICON | SHGFI_SYSICONINDEX | 
		SHGFI_USEFILEATTRIBUTES);

	return sfi.iIcon;
}

void CscsclientDlg::OnBnClickedButtonAdd()
{
	if (m_bShowBucket)
	{
		CCreateBucketDlg dlg(_T("Create Bucket"), _T("Bucket Name:"));
		dlg.DoModal();
	}
	else
	{
		CCreateBucketDlg dlg(_T("Create Folder"), _T("Folder Name:"));
		dlg.DoModal();
	}
}


void CscsclientDlg::OnBnClickedButtonRemove()
{
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	int iSel = m_list.GetNextSelectedItem(pos);
	if (iSel != -1)
	{
		CString name = m_list.GetItemText(iSel, 0);
		if (m_bShowBucket)
		{
			DeleteBucket(name);
		}
		else
		{
			CString objname = m_strDir;
			objname += name;
			DeleteObject(m_strBucket, objname);
		}
	}
}


void CscsclientDlg::OnBnClickedButtonRefresh()
{
	if (m_bShowBucket)
	{
		ListService();
	}
	else
	{
		ListBucket(m_strBucket, m_strDir);
	}
}


void CscsclientDlg::OnBnClickedButtonUpload()
{
	if (!m_bShowBucket)
	{
		CFileDialog filedlg(TRUE);
		int retval = filedlg.DoModal();
		if (retval == IDOK)
		{
			CString filepath = filedlg.GetPathName();
			CString filename = GetCurrentFolder() + filedlg.GetFileName();
			Upload(m_strBucket, filename, filepath, S3CannedAclPrivate);
		}
	}
}

const CString& CscsclientDlg::GetCurrentFolder()
{
	return m_strDir;
}

void CscsclientDlg::AddTask( TaskBase* task )
{
	m_Tasks.AddTail(task);
	m_OptDlg.Add(task->_type);
}

void CscsclientDlg::AddPriorTask( TaskBase* task )
{
	POSITION pos = m_Tasks.GetHeadPosition();
	if (pos != NULL &&
		m_Tasks.GetNext(pos)->IsRunning())
	{
		m_Tasks.InsertAfter(pos, task);
	}
	else
	{
		m_Tasks.AddHead(task);
	}
}

void CscsclientDlg::AddUploadTask( UploadTask* task )
{
	m_UploadTasks.AddTail(task);
	m_dlgTransList.AddItem(task->_id, task->_filename, TRUE, 0);
}

void CscsclientDlg::AddDownloadTask( DownloadTask* task )
{
	m_DownloadTasks.AddTail(task);
	m_dlgTransList.AddItem(task->_id, task->_filename, FALSE, 0);
}

void CscsclientDlg::RunNextTask()
{
	if (!m_Tasks.IsEmpty())
	{
		TaskBase* task = m_Tasks.GetHead();
		if (task != NULL && !task->IsRunning())
		{
			task->Run();
		}
	}
}

void CscsclientDlg::RunNextUploadTask()
{
	if (!m_UploadTasks.IsEmpty())
	{
		int nRunning = 0;
		POSITION pos = m_UploadTasks.GetHeadPosition();
		while (pos != NULL &&
			nRunning < MaxCountRunningUpload)
		{
			TaskBase* task = m_UploadTasks.GetNext(pos);
			ASSERT(task != NULL);
			if (!task->IsRunning())
			{
				task->Run();
			}
			nRunning ++;
		}
	}
}

void CscsclientDlg::RunNextDownloadTask()
{
	if (!m_DownloadTasks.IsEmpty())
	{
		int nRunning = 0;
		POSITION pos = m_DownloadTasks.GetHeadPosition();
		while (pos != NULL &&
			nRunning < MaxCountRunningDownload)
		{
			TaskBase* task = m_DownloadTasks.GetNext(pos);
			ASSERT(task != NULL);
			if (!task->IsRunning())
			{
				task->Run();
			}
			nRunning ++;
		}
	}
}

void CscsclientDlg::PopTask()
{
	if (!m_Tasks.IsEmpty())
	{
		TaskBase* task = m_Tasks.RemoveHead();
		ASSERT(task->IsRunning());
		delete task;
		m_OptDlg.Done(TRUE);
	}
}

void CscsclientDlg::PopUploadTask( UINT64 id )
{
	if (!m_UploadTasks.IsEmpty())
	{
		POSITION pos = m_UploadTasks.GetHeadPosition();
		while (pos != NULL)
		{
			POSITION pre_pos = pos;
			TaskBase* task = m_UploadTasks.GetNext(pos);
			ASSERT(task != NULL);
			if (task->_id == id)
			{
				delete task;
				m_UploadTasks.RemoveAt(pre_pos);
				break;
			}
		}
	}
}

void CscsclientDlg::PopDownloadTask( UINT64 id )
{
	if (!m_DownloadTasks.IsEmpty())
	{
		POSITION pos = m_DownloadTasks.GetHeadPosition();
		while (pos != NULL)
		{
			POSITION pre_pos = pos;
			TaskBase* task = m_DownloadTasks.GetNext(pos);
			ASSERT(task != NULL);
			if (task->_id == id)
			{
				delete task;
				m_DownloadTasks.RemoveAt(pre_pos);
				break;
			}
		}
	}
}

void CscsclientDlg::ListService()
{
	AddPriorTask(new ListServiceTask);
	RunNextTask();
}

void CscsclientDlg::ListBucket(const CString& buckname, const CString& prefix)
{
	AddPriorTask(new ListBucketTask(buckname, prefix));
	RunNextTask();
}

void CscsclientDlg::CreateBucket( const CString& buckname, S3CannedAcl acl )
{
	AddTask(new CreateBucketTask(buckname, acl));
	RunNextTask();
}

void CscsclientDlg::CreateFolder( const CString& buckname, const CString& foldername, S3CannedAcl acl )
{
	AddTask(new CreateFolderTask(buckname, foldername, acl));
	RunNextTask();
}

void CscsclientDlg::DeleteBucket( const CString& buckname )
{
	AddTask(new DeleteBucketTask(buckname));
	RunNextTask();
}

void CscsclientDlg::DeleteObject( const CString& buckname, const CString& objname )
{
	AddTask(new DeleteObjectTask(buckname, objname));
	RunNextTask();
}

void CscsclientDlg::MetaBucket( const CString& buckname )
{
	AddTask(new MetaTask(buckname));
	RunNextTask();
}

void CscsclientDlg::SetAcl( const CString& buckname, const CString& objname, std::map<CString, char>& acls )
{
	AddTask(new AclTask(buckname, objname, acls));
	RunNextTask();
}

void CscsclientDlg::GetAcl( const CString& buckname, const CString& objname, ListBucketItem* pItem )
{
	AddTask(new GetAclTask(buckname, objname, pItem));
	RunNextTask();
}

void CscsclientDlg::Upload( const CString& buckname, const CString& filename, 
						   const CString& localpath, S3CannedAcl acl )
{
	AddUploadTask(new UploadTask(buckname, filename, localpath, acl));
	RunNextUploadTask();
}

void CscsclientDlg::Download( const CString& buckname, const CString& filename, const CString& localpath )
{
	AddDownloadTask(new DownloadTask(buckname, filename, localpath));
	RunNextDownloadTask();
}

void CscsclientDlg::Copy( const CString& buckname, const CString& frompath, const CString& topath )
{
	AddTask(new CopyTask(buckname, frompath, topath));
	RunNextTask();
}

void CscsclientDlg::Rename( const CString& buckname, const CString& frompath, const CString& topath )
{
	AddTask(new RenameTask(buckname, frompath, topath));
	RunNextTask();
}

LRESULT CscsclientDlg::OnCreateBucketResult( WPARAM wParam, LPARAM lParam )
{
	if (wParam == 0)
	{
		unsigned long status = lParam;
		if (status / 100 == 2)
		{
			OnBnClickedButtonRefresh();
		}
		else
		{
			// error
		}
	}
	else
	{
		// curl error
	}
	PopTask();
	RunNextTask();
	return S_OK;
}

LRESULT CscsclientDlg::OnCreateFolderResult( WPARAM wParam, LPARAM lParam )
{
	if (wParam == 0)
	{
		unsigned long status = lParam;
		if (status / 100 == 2)
		{
			OnBnClickedButtonRefresh();
		}
		else
		{
			// error
		}
	}
	else
	{
		// curl error
	}
	PopTask();
	RunNextTask();
	return S_OK;
}

LRESULT CscsclientDlg::OnDeleteBucketResult( WPARAM wParam, LPARAM lParam )
{
	if (wParam == 0)
	{
		unsigned long status = lParam;
		if (status / 100 == 2)
		{
			OnBnClickedButtonRefresh();
		}
		else
		{
			// error
		}
	}
	else
	{
		// curl error
	}
	PopTask();
	RunNextTask();
	return S_OK;
}

LRESULT CscsclientDlg::OnDeleteObjectResult( WPARAM wParam, LPARAM lParam )
{
	if (wParam == 0)
	{
		unsigned long status = lParam;
		if (status / 100 == 2)
		{
			OnBnClickedButtonRefresh();
		}
		else
		{
			// error
		}
	}
	else
	{
		// curl error
	}
	PopTask();
	RunNextTask();
	return S_OK;
}


void CscsclientDlg::OnBnClickedButtonShowlog()
{
	BOOL bChecked = m_btnTasks.GetCheck();
	if (bChecked)
	{
		m_btnTasks.SetWindowText(_T("Hide"));
	}
	else
	{
		m_btnTasks.SetWindowText(_T("Show"));
	}
	ShowOptDlg(bChecked);
}

LRESULT CscsclientDlg::OnUploadResult( WPARAM wParam, LPARAM lParam )
{
	if (wParam == 0)
	{
		UploadResult* ret = (UploadResult*)lParam;
		unsigned long status = ret->status;
		UINT64 id = ret->id;
		if (status / 100 == 2)
		{
			OnBnClickedButtonRefresh();
		}
		else
		{
			// error
			if (status < 100)
			{
				ShowInfoMsg(string2CString(multi_http::curl_error_string(status)));
			}
		}
		delete ret;
		PopUploadTask(id);
		m_dlgTransList.DelItem(id, TRUE);
	}
	else
	{
		// curl error
	}
	RunNextDownloadTask();
	return S_OK;
}

LRESULT CscsclientDlg::OnDownloadResult( WPARAM wParam, LPARAM lParam )
{
	if (wParam == 0)
	{
		DownloadResult* ret = (DownloadResult*)lParam;
		unsigned long status = ret->status;
		UINT64 id = ret->id;
		if (status / 100 == 2)
		{

		}
		else
		{
			// error
		}
		delete ret;
		ShowDownloadInfo();
		PopDownloadTask(id);
		m_dlgTransList.DelItem(id, FALSE);
	}
	else
	{
		// curl error
	}
	RunNextDownloadTask();
	return S_OK;
}

LRESULT CscsclientDlg::OnMetaResult( WPARAM wParam, LPARAM lParam )
{
	if (wParam == 0)
	{
		MetaResult* ret = (MetaResult*)lParam;
		if (ret->status / 100 == 2)
		{
			if (!::IsWindow(m_dlgMeta.GetSafeHwnd()))
			{
				m_dlgMeta.Create(CMetaDlg::IDD, this);
			}
			m_dlgMeta.ShowWindow(SW_SHOW);
			m_dlgMeta.ClearAll();
			m_dlgMeta.SetBucketName(ret->buckname);

			std::map<std::string, std::string>::iterator it_attr = 
				ret->results.begin();
			for (; it_attr != ret->results.end(); ++ it_attr)
			{
				m_dlgMeta.AddAttribute(string2CString(it_attr->first),
					string2CString(it_attr->second));
			}

			char acl_c = 0;
			char acl_a = 0;
			std::map<std::string, char>::iterator it_c =
				ret->acls.find(CANONICAL_GROUP);
			if (it_c != ret->acls.end())
			{
				acl_c = it_c->second;
			}
			std::map<std::string, char>::iterator it_a =
				ret->acls.find(ANONYMOUSE_GROUP);
			if (it_a != ret->acls.end())
			{
				acl_a = it_a->second;
			}
			m_dlgMeta.SetCanonicalGroupAcl(acl_c);
			m_dlgMeta.SetAnonymouseGroupAcl(acl_a);

			std::map<std::string, char>::iterator it =
				ret->acls.begin();
			for (; it != ret->acls.end(); ++ it)
			{
				if (it->first != ANONYMOUSE_GROUP &&
					it->first != CANONICAL_GROUP)
				{
					m_dlgMeta.AddUserAcl(string2CString(it->first), it->second);
				}
			}
		}
		else
		{
			// error
		}
		delete ret;
	}
	else
	{
		// curl error
	}
	PopTask();
	RunNextTask();
	return S_OK;
}

LRESULT CscsclientDlg::OnAclResult( WPARAM wParam, LPARAM lParam )
{
	if (wParam == 0)
	{
		unsigned long status = lParam;
		if (status / 100 == 2)
		{
		}
		else
		{
			// error
		}
	}
	else
	{
		// curl error
	}
	PopTask();
	RunNextTask();
	return S_OK;
}

LRESULT CscsclientDlg::OnGetAclResult( WPARAM wParam, LPARAM lParam )
{
	if (wParam == 0)
	{
		GetAclResult* ret = (GetAclResult*)lParam;
		if (ret->status / 100 == 2)
		{
			if (!::IsWindow(m_dlgInfo.GetSafeHwnd()))
			{
				m_dlgInfo.Create(CFileInfoDlg::IDD, this);
			}
			m_dlgInfo.ShowWindow(SW_SHOW);
			m_dlgInfo.ClearAll();
			m_dlgInfo.SetBucketName(ret->buckname);
			m_dlgInfo.SetObjcetName(ret->objname);

			m_dlgInfo.AddAttribute(_T("Owner"), ret->owner);
			m_dlgInfo.AddAttribute(_T("Size"), ret->item.size_str);
			m_dlgInfo.AddAttribute(_T("Modifid Time"), ret->item.date);
			m_dlgInfo.AddAttribute(_T("MD5"), ret->item.md5);

			char acl_c = 0;
			char acl_a = 0;
			std::map<std::string, char>::iterator it_c =
				ret->acls.find(CANONICAL_GROUP);
			if (it_c != ret->acls.end())
			{
				acl_c = it_c->second;
			}
			std::map<std::string, char>::iterator it_a =
				ret->acls.find(ANONYMOUSE_GROUP);
			if (it_a != ret->acls.end())
			{
				acl_a = it_a->second;
			}
			m_dlgInfo.SetCanonicalGroupAcl(acl_c);
			m_dlgInfo.SetAnonymouseGroupAcl(acl_a);

			std::map<std::string, char>::iterator it =
				ret->acls.begin();
			for (; it != ret->acls.end(); ++ it)
			{
				if (it->first != ANONYMOUSE_GROUP &&
					it->first != CANONICAL_GROUP)
				{
					m_dlgInfo.AddUserAcl(string2CString(it->first), it->second);
				}
			}
		}
		else
		{
			// error
		}
		delete ret;
	}
	else
	{
		// curl error
	}
	PopTask();
	RunNextTask();
	return S_OK;
}

LRESULT CscsclientDlg::OnCopyResult( WPARAM wParam, LPARAM lParam )
{
	if (wParam == 0)
	{
		unsigned long status = lParam;
		if (status / 100 == 2)
		{
			OnBnClickedButtonRefresh();
		}
		else
		{
			// error
		}
	}
	else
	{
		// curl error
	}
	PopTask();
	RunNextTask();
	return S_OK; 
}

LRESULT CscsclientDlg::OnRenameResult( WPARAM wParam, LPARAM lParam )
{
	if (wParam == 0)
	{
		RenameResult* ret = (RenameResult*)lParam;
		if (ret->status / 100 == 2)
		{
			AddPriorTask(new DeleteObjectTask(ret->buckname, ret->fromname));
		}
		else
		{
			// error
		}
		delete ret;
	}
	else
	{
		// curl error
	}
	PopTask();
	RunNextTask();
	return S_OK; 
}

void CscsclientDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	OrderDlgItemPos();
}

void CscsclientDlg::ShowInfoMsg( const CString& msg )
{
	CWnd* pStatic = GetDlgItem(IDC_STATIC_INFO);
	ASSERT(pStatic != NULL);
	pStatic->SetWindowText(msg);
}

void CscsclientDlg::ShowPathMsg()
{
	CString strMsg = _T("Path: ");
	strMsg.Append(m_strBucket);
	strMsg.Append(_T("/"));
	strMsg.Append(m_strDir);
	ShowInfoMsg(strMsg);
}

void CscsclientDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialogEx::OnSizing(fwSide, pRect);

	if (pRect->right - pRect->left < MainDlgMinWidth)
		pRect->right =  pRect->left + MainDlgMinWidth;
	if (pRect->bottom - pRect->top < MainDlgMinHeigth)
		pRect->bottom = pRect->top + MainDlgMinHeigth;
}

void CscsclientDlg::ShowOptDlg( BOOL bShow )
{
	if (bShow)
	{
		if (!m_dlgTransList.IsWindowVisible())
		{
			m_dlgTransList.ShowWindow(SW_SHOW);
			OrderDlgItemPos();
		}
	}
	else
	{
		if (m_dlgTransList.IsWindowVisible())
		{
			m_dlgTransList.ShowWindow(SW_HIDE);
			OrderDlgItemPos();
		}
	}
}

void CscsclientDlg::OrderDlgItemPos()
{
	int cx, cy;
	CRect rtClient;
	GetClientRect(rtClient);
	cx = rtClient.Width();
	cy = rtClient.Height();

	CRect rtList, rtStatic;
	CRect rtInfo(0, 0, 0, 0);

	CWnd* pStatic = GetDlgItem(IDC_STATIC_INFO);
	ASSERT(pStatic != NULL);
	pStatic->GetWindowRect(&rtStatic);
	this->ScreenToClient(&rtStatic);

	m_list.GetWindowRect(&rtList);
	this->ScreenToClient(&rtList);

	// operation dlg
	if (m_dlgTransList.IsWindowVisible())
	{
		m_dlgTransList.GetWindowRect(rtInfo);
		this->ScreenToClient(rtInfo);
		m_dlgTransList.SetWindowPos(&wndTop, cx - rtInfo.Width(), rtList.top, 
			rtInfo.Width(), cy - rtList.top - rtStatic.Height(),
			SWP_NOZORDER | SWP_SHOWWINDOW);
	}

	// info static
	pStatic->SetWindowPos(&wndTop, 0, cy - rtStatic.Height(), cx, rtStatic.Height(),
		SWP_NOZORDER | SWP_SHOWWINDOW);

	// list ctrl
	m_list.SetWindowPos(&wndTop, 0, 0, cx - rtInfo.Width() - 2, cy - rtList.top - rtStatic.Height(),
		SWP_NOMOVE | SWP_SHOWWINDOW);

	CRect rtOptBtn;
	m_btnTasks.GetWindowRect(&rtOptBtn);
	this->ScreenToClient(&rtOptBtn);
	m_btnTasks.SetWindowPos(NULL, cx - rtOptBtn.Width(), rtClient.top + MainDlgButtonUpSpace, 0, 0, 
		SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
}

void CscsclientDlg::OnBnClickedButtonDownload()
{
	if (m_bShowBucket)
		return;

	POSITION pos = m_list.GetFirstSelectedItemPosition();
	int iSel = m_list.GetNextSelectedItem(pos);
	if (iSel == -1)
		return;

	CString name = m_list.GetItemText(iSel, 0);
	if (name.ReverseFind(_T('/')) != -1)
		return;

	CFileDialog dlgFile(FALSE, 0, name);
	if (IDOK == dlgFile.DoModal())
	{
		CString localpath = dlgFile.GetPathName();

		WIN32_FIND_DATA fd;
		HANDLE hFind = FindFirstFile(localpath, &fd);
		if (hFind != INVALID_HANDLE_VALUE)
		{ //文件已存在
			AfxMessageBox(_T("File Already Exist!"));
		}
		else
		{
			Download(m_strBucket, m_strDir + name, localpath);
		}
	}
}

void CscsclientDlg::OnBnClickedButtonRename()
{
	if (!m_bShowBucket)
	{
		POSITION pos = m_list.GetFirstSelectedItemPosition();
		int iSel = m_list.GetNextSelectedItem(pos);
		if (iSel != -1)
		{
			m_list.SetFocus();
			m_list.EditLabel(iSel);
		}
	}
}

void CscsclientDlg::OrderServerListButtons()
{
	m_btnUpload.ShowWindow(SW_HIDE);
	m_btnDown.ShowWindow(SW_HIDE);
	m_btnRename.ShowWindow(SW_HIDE);

	CRect rtClient, rtButton;
	GetClientRect(rtClient);
	m_btnAdd.GetWindowRect(rtButton);

	int y = rtClient.top + MainDlgButtonUpSpace;
	int x = rtButton.Width() + MainDlgButtonInterval;
	m_btnAdd.SetWindowPos(NULL, rtClient.left, y, 0, 0, 
		SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
	m_btnRemove.SetWindowPos(NULL, rtClient.left + x, y, 0, 0, 
		SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
	m_btnRefresh.SetWindowPos(NULL, rtClient.left + x * 2, y, 0, 0, 
		SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);

	PostMessage(WM_SIZE); // 刷新窗口
}

void CscsclientDlg::OrderBucketListButtons()
{
	CRect rtClient, rtButton;
	GetClientRect(rtClient);
	m_btnAdd.GetWindowRect(rtButton);

	int y = rtClient.top + MainDlgButtonUpSpace;
	int x = rtButton.Width() + MainDlgButtonInterval;
	m_btnAdd.SetWindowPos(NULL, rtClient.left, y, 0, 0, 
		SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
	m_btnUpload.SetWindowPos(NULL, rtClient.left + x, y, 0, 0, 
		SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
	m_btnDown.SetWindowPos(NULL, rtClient.left + x * 2, y, 0, 0, 
		SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
	m_btnRename.SetWindowPos(NULL, rtClient.left + x * 3, y, 0, 0, 
		SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
	m_btnRemove.SetWindowPos(NULL, rtClient.left + x * 4, y, 0, 0, 
		SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
	m_btnRefresh.SetWindowPos(NULL, rtClient.left + x * 5, y, 0, 0, 
		SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
}

void CscsclientDlg::post_message( unsigned long msg_id, unsigned long param1 /*= 0*/, unsigned long param2 /*= 0*/ )
{
	CurlMsgParams* params = new CurlMsgParams;
	params->msg_id = msg_id;
	params->param1 = param1;
	params->param2 = param2;
	this->PostMessage(WM_MAINDLG_MULITCURL, (WPARAM)params);
}

LRESULT CscsclientDlg::OnCancelTrans( WPARAM wParam, LPARAM lParam )
{
	UINT64 id = (UINT64)wParam;
	BOOL bUpload = (BOOL)lParam;
	if (bUpload)
	{
		CancelUploadTask(id);
		PopUploadTask(id);
	}
	else
	{
		CancelDownloadTask(id);
		PopDownloadTask(id);
	}
	m_dlgTransList.DelItem(id, bUpload);
	return S_OK;
}

void CscsclientDlg::CancelUploadTask( UINT64 id )
{
	POSITION pos = m_UploadTasks.GetHeadPosition();
	while (pos != NULL)
	{
		TaskBase* task = m_UploadTasks.GetNext(pos);
		if (task && task->_id == id)
		{
			task->Cancel();
			break;
		}
	}
}

void CscsclientDlg::CancelDownloadTask( UINT64 id )
{
	POSITION pos = m_DownloadTasks.GetHeadPosition();
	while (pos != NULL)
	{
		TaskBase* task = m_DownloadTasks.GetNext(pos);
		if (task && task->_id == id)
		{
			task->Cancel();
			break;
		}
	}
}

void CscsclientDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case ID_RunTaskTimeout:
		{
			RunNextTask();
			RunNextUploadTask();
			RunNextDownloadTask();
		}
		break;

	case ID_ShowTaskTimeout:
		{
			ShowUploadInfo();
			ShowDownloadInfo();
		}
		break;

	case ID_ShowMsgTimeout:
		{
			
		}
		break;

	default:
		ASSERT(FALSE);
		break;
	}

	__super::OnTimer(nIDEvent);
}

void CscsclientDlg::ShowUploadInfo()
{
	if (!m_UploadTasks.IsEmpty())
	{
		int nRunning = 0;
		POSITION pos = m_UploadTasks.GetHeadPosition();
		while (pos != NULL &&
			nRunning < MaxCountRunningUpload)
		{
			UploadTask* task = (UploadTask*)m_UploadTasks.GetNext(pos);
			ASSERT(task != NULL);
			if (task->IsRunning())
			{
				nRunning ++;
				m_dlgTransList.SetTransing(task->_id, TRUE, TRUE);
				m_dlgTransList.SetFileSize(task->_id, TRUE, task->file_size());
				m_dlgTransList.SetTransSize(task->_id, TRUE, task->_trans_total);
				m_dlgTransList.SetTransRate(task->_id, TRUE, task->_speed_calc.speed_value());
			}
		}
	}
}

void CscsclientDlg::ShowDownloadInfo()
{
	if (!m_DownloadTasks.IsEmpty())
	{
		int nRunning = 0;
		POSITION pos = m_DownloadTasks.GetHeadPosition();
		while (pos != NULL &&
			nRunning < MaxCountRunningDownload)
		{
			DownloadTask* task = (DownloadTask*)m_DownloadTasks.GetNext(pos);
			ASSERT(task != NULL);
			if (task->IsRunning())
			{
				nRunning ++;
				m_dlgTransList.SetTransing(task->_id, FALSE, TRUE);
				m_dlgTransList.SetFileSize(task->_id, FALSE, task->file_size());
				m_dlgTransList.SetTransSize(task->_id, FALSE, task->_trans_total);
				m_dlgTransList.SetTransRate(task->_id, FALSE, task->_speed_calc.speed_value());
			}
		}
	}
}


void CscsclientDlg::OnBusketlistMetainfo()
{
	if (m_bShowBucket)
	{
		POSITION pos = m_list.GetFirstSelectedItemPosition();
		int iSel = m_list.GetNextSelectedItem(pos);
		if (iSel != -1)
		{
			CString name = m_list.GetItemText(iSel, 0);
			MetaBucket(name);
		}
	}
}



void CscsclientDlg::OnFilelistInfo()
{
	if (m_bShowBucket)
		return;

	POSITION pos = m_list.GetFirstSelectedItemPosition();
	int iSel = m_list.GetNextSelectedItem(pos);
	if (iSel == -1)
		return;

	CString name = m_list.GetItemText(iSel, 0);
 	CString size = m_list.GetItemText(iSel, 1);
 	CString mtime = m_list.GetItemText(iSel, 2);
 	CString md5 = m_list.GetItemText(iSel, 3);

	ListBucketItem item;
	item.name = name;
	item.date = mtime;
	item.size_str = size;
	item.md5 = md5;

	GetAcl(m_strBucket, m_strDir + name, &item);
}

void CscsclientDlg::ClearListContents()
{
	while (m_list.DeleteItem(0)) {}
}

void CscsclientDlg::ClearListColumns()
{
	while (m_list.DeleteColumn(0)) {}
}

void CscsclientDlg::OnLvnBeginlabeleditList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (m_bShowBucket)
	{
		*pResult = 1;
	}
	else
	{
		NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
		m_nEditItem = pDispInfo->item.iItem;
		CString strName = m_list.GetItemText(m_nEditItem, 0);
		if (strName.ReverseFind(_T('/')) != -1)
			*pResult = 1;
		else
			*pResult = 0;
	}
}

void CscsclientDlg::OnLvnEndlabeleditList1(NMHDR *pNMHDR, LRESULT *pResult)
{
 	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	LVITEM* plvItem = &pDispInfo->item;

	if (plvItem->pszText != NULL)
	{
		CString toname = plvItem->pszText;
		CString fromname = m_list.GetItemText(m_nEditItem, 0);
		CString newLow = toname;
		CString oldLow = fromname;
		if (newLow.MakeLower() != oldLow.MakeLower())
		{
			Rename(m_strBucket, m_strDir + fromname, m_strDir + toname);
		}
	}
	*pResult = FALSE;
}

void CscsclientDlg::SaveConfigInfo( ConfigInfo& info )
{
	ASSERT(!m_strConfigPath.IsEmpty());
	CString filepath = m_strConfigPath;
	filepath.Append(_T("\\"));
	filepath.Append(_T(CONFIG_FILE_NAME));

	CString strTemp;
	strTemp.Format(_T("%d"), info.bSaveSecret);
	::WritePrivateProfileString(_T(CONFIG_FILE_APP), _T(CONFIG_SAVE_SECRET_KEY),
		strTemp, filepath);
	strTemp.Format(_T("%d"), info.bUseHttps);
	::WritePrivateProfileString(_T(CONFIG_FILE_APP), _T(CONFIG_USE_HTTPS_KEY),
		strTemp, filepath);
	::WritePrivateProfileString(_T(CONFIG_FILE_APP), _T(CONFIG_ACCESS_KEY),
		info.strAccessKey, filepath);
	std::string sEncrypt = CString2string(info.strAccessSecret);
	sEncrypt = EncryptString(sEncrypt);
	::WritePrivateProfileStringA(CONFIG_FILE_APP, CONFIG_SECRET_KEY,
		sEncrypt.c_str(), CString2string(filepath).c_str());
}

void CscsclientDlg::LoadConfigInfo( ConfigInfo& info )
{
	ASSERT(!m_strConfigPath.IsEmpty());
	CString filepath = m_strConfigPath;
	filepath.Append(_T("\\"));
	filepath.Append(_T(CONFIG_FILE_NAME));

	info.bSaveSecret = ::GetPrivateProfileInt(_T(CONFIG_FILE_APP), 
		_T(CONFIG_SAVE_SECRET_KEY), 0, filepath);
	info.bUseHttps = ::GetPrivateProfileInt(_T(CONFIG_FILE_APP), 
		_T(CONFIG_USE_HTTPS_KEY), 1, filepath);
	::GetPrivateProfileString(_T(CONFIG_FILE_APP), _T(CONFIG_ACCESS_KEY), 
		_T(""), info.strAccessKey.GetBufferSetLength(128), 128, filepath);
	info.strAccessKey.ReleaseBuffer();

	char szDecrytp[128] = {0};
	::GetPrivateProfileStringA(CONFIG_FILE_APP, CONFIG_SECRET_KEY, 
		"", szDecrytp, 128, CString2string(filepath).c_str());
	std::string sDecrypt = DecryptString(szDecrytp);
	info.strAccessSecret = string2CString(sDecrypt);
}
