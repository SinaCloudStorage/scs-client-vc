
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展





#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


#include "Log4cplusSwitch.h"
#include <string>
#include <vector>


#ifndef snprintf
#	define snprintf _snprintf
#endif // !snprintf

#define BucketNameColumnWidth			400
#define BucketTimeColumnWidth			400

#define FileNameColumnWidth				280
#define FileSizeColumnWidth				100
#define ModifedTimeColumnWidth			200
#define FileMd5ColumnWidth				400

#define MainDlgMinWidth					680
#define MainDlgMinHeigth				280

#define MainDlgButtonUpSpace			8
#define MainDlgButtonInterval			8

#define MaxCountRunningUpload			3
#define MaxCountRunningDownload			3

#define S3_HOSTNAME ""

enum TaskType
{
	kListService,
	kListBucket,
	kDelectBucket,
	kDeleteObject,
	kCreateBucket,
	kCreateFolder,
	kUpload,
	kDownload,
	kMeta,
	kAcl,
	kGetAcl,
	kCopy,
	kRename,
};

#define WM_BASE							WM_USER+2000
#define WM_MAINDLG_MULITCURL			WM_USER+198
#define WM_MAINDLG_LOGIN				WM_USER+199
#define WM_MAINDLG_CREATEITEM			WM_USER+200
#define WM_MAINDLG_CancelTrans			WM_BASE+201
#define WM_MAINDLG_ChangeAcl			WM_BASE+202

#define WM_MAINDLG_ListServiceResult	WM_BASE+kListService
#define WM_MAINDLG_ListBucketResult		WM_BASE+kListBucket
#define WM_MAINDLG_CreateBucketResult	WM_BASE+kCreateBucket
#define WM_MAINDLG_CreateFolderResult	WM_BASE+kCreateFolder
#define WM_MAINDLG_DeleteBucketResult	WM_BASE+kDelectBucket
#define WM_MAINDLG_DeleteObjectResult	WM_BASE+kDeleteObject
#define WM_MAINDLG_UploadResult			WM_BASE+kUpload
#define WM_MAINDLG_DownloadResult		WM_BASE+kDownload
#define WM_MAINDLG_MetaResult			WM_BASE+kMeta
#define WM_MAINDLG_AclResult			WM_BASE+kAcl
#define WM_MAINDLG_GetAclResult			WM_BASE+kGetAcl
#define WM_MAINDLG_CopyResult			WM_BASE+kCopy
#define WM_MAINDLG_RenameResult			WM_BASE+kRename

typedef enum
{
	S3CannedAclPrivate                  = 0, /* private */
	S3CannedAclPublicRead               = 1, /* public-read */
	S3CannedAclPublicReadWrite          = 2, /* public-read-write */
	S3CannedAclAuthenticatedRead        = 3  /* authenticated-read */
} S3CannedAcl;

struct ConfigInfo
{
	ConfigInfo()
		: strAccessKey(_T(""))
		, strAccessSecret(_T(""))
		, bUseHttps(TRUE)
		, bSaveSecret(FALSE)
	{}

	CString			strAccessKey;
	CString			strAccessSecret;
	BOOL			bUseHttps;
	BOOL			bSaveSecret;
};

typedef ConfigInfo LoginParams;

struct ListServiceItem
{
	CString				name;
	CString				date;
	unsigned long long	bytes;
};

struct ListServiceResult
{
	long							status;
	CString							display_name;
	CString							id;
	std::vector<ListServiceItem>	items;
};

struct ListBucketItem
{
	CString				name;
	bool				is_dir;
	CString				md5;
	CString				sha1;
	CString				date;
	unsigned long long	size;
	CString				size_str;
	CString				owner;
};

struct ListBucketResult
{
	long status;
	CString buckname;
	CString foldername;
	std::vector<ListBucketItem> items;
};

struct UploadResult
{
	long status;
	UINT64 id;
};

struct DownloadResult
{
	long status;
	UINT64 id;
};

struct MetaResult
{
	long status;
	CString buckname;
	std::map<std::string, std::string> results;
	std::map<std::string, char> acls;
};

struct GetAclResult
{
	long status;
	CString buckname;
	CString objname;
	CString owner;
	ListBucketItem item;
	std::map<std::string, char> acls;
};

struct RenameResult
{
	long status;
	CString buckname;
	CString fromname;
	CString toname;
};

struct CurlMsgParams
{
	unsigned long msg_id;
	unsigned long param1;
	unsigned long param2;
};

struct UploadInfo
{
	int					task_id;
	unsigned long long	total_size;
	unsigned long long	trans_size;
	unsigned long		speed;
};

struct ChangeAclParams
{
	CString buckname;
	CString objname;
	std::map<CString, char> acls;
};

#ifndef FOPEN_EXTRA_FLAGS
#define FOPEN_EXTRA_FLAGS ""
#endif

/**
 * S3_EXPIRES_TIMEOFFSET applied to x-amz-date
 **/
#define S3_EXPIRES_TIMEOFFSET              300

#define S3_DEFAULT_HOSTNAME					"sinastorage.cn"
#define S3_DEFAULT_HOSTNAME2                "sinacloud.net"
#define DOWNLOAD_TEMP						".stemp"
#define DOWNLOAD_CFG						".scfg"

#define ANONYMOUSE_GROUP					"GRPS000000ANONYMOUSE"
#define CANONICAL_GROUP						"GRPS0000000CANONICAL"

#define ANONYMOUSE_GROUP_SHOWNAME			"Anonymouse Group"
#define CANONICAL_GROUP_SHOWNAME			"Canonical Group"

#define ACL_COUNT							4

#define CONFIG_FILE_NAME					"scs-config.ini"
#define CONFIG_FILE_APP						"SCS_CONFIG"
#define CONFIG_ACCESS_KEY					"access"
#define CONFIG_SECRET_KEY					"socret"
#define CONFIG_USE_HTTPS_KEY				"https"
#define CONFIG_SAVE_SECRET_KEY				"save"

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

