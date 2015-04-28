#pragma once

#include "assert.h"
#include "http_handler_base.h"
#include "singleton.h"
#include "multi_http.h"
#include "speed_calculator.h"

class TaskBase : public http_handler_base
{
	typedef std::map<std::string, std::string> StringMap;
public:
	TaskBase();
	BOOL IsRunning() { return _running; }
	void Run();
	void Cancel();
	virtual void handle_curl_completed(const std::string& response, long status) {}
	virtual void handle_curl_error(int error_code);

protected:
	virtual void RunImp() = 0;
	std::string compose_auth_header(
		const std::string& http_method,
		StringMap& amz_date,
		const std::string& resource,
		const std::string& data, 
		const std::string& content_type = "",
		const std::string& content_md5 = "");

public:
	TaskType _type;
	BOOL _running;
	UINT64 _id;
	static UINT64 _maxid;
	static std::string _s_access_key;
	static std::string _s_secret_key;

	std::string _date;
	std::string _content_type;
	StringMap _amz_data;
};

class ListServiceTask : public TaskBase
{
public:
	ListServiceTask() { _type = kListService; }

	virtual void handle_curl_completed(const std::string& response, long status);

protected:
	virtual void RunImp()
	{}
};

class ListBucketTask : public TaskBase
{
public:
	ListBucketTask(const CString& buckname, const CString& prefix) 
		: _buckname(buckname)
		, _prefix(prefix)
	{ _type = kListBucket; }

	virtual void handle_curl_completed(const std::string& response, long status);

protected:
	virtual void RunImp();

	CString _buckname;
	CString _prefix;
};

class CreateBucketTask : public TaskBase
{
public:
	CreateBucketTask(const CString& buckname, S3CannedAcl acl)
		: _buckname(buckname)
		, _acl(acl)
	{ _type = kCreateBucket; }

	virtual void handle_curl_completed(const std::string& response, long status);

protected:
	virtual void RunImp();

public:
	CString _buckname;
	S3CannedAcl _acl;
};

class CreateFolderTask : public TaskBase
{
public:
	CreateFolderTask(const CString& buckname, const CString& foldername, S3CannedAcl acl)
		: _buckname(buckname)
		, _foldername(foldername)
		, _acl(acl)
	{ _type = kCreateFolder; }

	virtual void handle_curl_completed(const std::string& response, long status);

protected:
	virtual void RunImp();

public:
	CString _buckname;
	CString _foldername;
	S3CannedAcl _acl;
};

class DeleteBucketTask : public TaskBase
{
public:
	DeleteBucketTask(const CString& buckname)
		: _buckname(buckname)
	{ _type = kDelectBucket; }

	virtual void handle_curl_completed(const std::string& response, long status);

protected:
	virtual void RunImp();

public:
	CString _buckname;
};

class DeleteObjectTask : public TaskBase
{
public:
	DeleteObjectTask(const CString& buckname, const CString& objname)
		: _buckname(buckname)
		, _objname(objname)
	{ _type = kDeleteObject; }

	virtual void handle_curl_completed(const std::string& response, long status);

protected:
	virtual void RunImp();

public:
	CString	_buckname;
	CString _objname;
};

class UploadTask : public TaskBase
{
public:
	UploadTask(const CString& buckname, const CString& filename,
		const CString& localpath, S3CannedAcl acl)
		: _buckname(buckname)
		, _filename(filename)
		, _localpath(localpath)
		, _acl(acl)
		, _speed_calc(200, 25)
		, _trans_total(0)
	{ _type = kUpload; }

	virtual void handle_curl_completed(const std::string& response, long status);
	virtual void handle_trans_bytes(uint64 trans_bytes, uint64 total);

protected:
	virtual void RunImp();

public:
	CString _buckname;
	CString _filename;
	CString _localpath;
	S3CannedAcl _acl;

	speed_calculator _speed_calc;
	unsigned long long _trans_total;
};

class DownloadTask : public TaskBase
{
public:
	DownloadTask(const CString& buckname, const CString& filename, const CString& localpath)
		: _buckname(buckname)
		, _filename(filename)
		, _localpath(localpath)
		, _speed_calc(200, 25)
		, _trans_total(0)
	{ _type = kDownload; }

	virtual void handle_curl_completed(const std::string& response, long status);
	virtual void handle_trans_bytes(uint64 trans_bytes, uint64 total);

protected:
	virtual void RunImp();

public:
	CString _buckname;
	CString _filename;
	CString _localpath;

	speed_calculator _speed_calc;
	unsigned long long _trans_total;
};

class MetaTask : public TaskBase
{
public:
	MetaTask(const CString& buckname)
		: _buckname(buckname)
	{ _type = kMeta; }

	virtual void handle_curl_completed(const std::string& response, long status);

protected:
	virtual void RunImp();

public:
	CString _buckname;
};

class AclTask : public TaskBase
{
public:
	AclTask(const CString& buckname, 
		const CString& objname,
		std::map<CString, char>& acls);

	virtual void handle_curl_completed(const std::string& response, long status);

protected:
	virtual void RunImp();

public:
	CString _buckname;
	CString _objname;
};

class GetAclTask : public TaskBase
{
public:
	GetAclTask(const CString& buckname, const CString& objname, ListBucketItem* pItem = NULL);

	virtual void handle_curl_completed(const std::string& response, long status);

protected:
	virtual void RunImp();

public:
	CString _buckname;
	CString _objname;
	ListBucketItem _item;
};

class CopyTask : public TaskBase
{
public:
	CopyTask(const CString& buckname, const CString& frompath, const CString& topath)
		: _buckname(buckname)
		, _frompath(frompath)
		, _topath(topath)
	{ _type = kCopy; }

	virtual void handle_curl_completed(const std::string& response, long status);

protected:
	virtual void RunImp();

public:
	CString _buckname;
	CString _frompath;
	CString _topath;
};

class RenameTask : public CopyTask
{
public:
	RenameTask(const CString& buckname, const CString& frompath, const CString& topath)
		: CopyTask(buckname, frompath, topath)
	{ _type = kRename; }

	virtual void handle_curl_completed(const std::string& response, long status);
};