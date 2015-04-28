#include "stdafx.h"
#include "TaskDef.h"
#include "util.h"
#include "json.h"
#include "api_helper.h"

std::string TaskBase::_s_access_key = "";
std::string TaskBase::_s_secret_key = "";

#define CMD_TASK_TIMEOUT_SECOND		20

//////////////////////////////////////////////////////////////////////////
// base
TaskBase::TaskBase() 
	: _running(FALSE)
	, _id(_maxid++)
	, _content_type("")
	, _date("")
{
	set_uri("/");
	set_use_https(false);
	set_host(S3_DEFAULT_HOSTNAME);
	set_timeout(CMD_TASK_TIMEOUT_SECOND);
	add_params("formatter", "json");
	add_header("Accept-Encoding", "identity");
}

std::string TaskBase::compose_auth_header( const std::string& http_method, 
										  StringMap& amz_date, 
										  const std::string& resource, 
										  const std::string& date, 
										  const std::string& content_type,
										  const std::string& content_md5 )
{
	unsigned char hmac[20];
	char b64[((20 + 1) * 4) / 3];

	std::string signbuf;
	signbuf += http_method;
	signbuf += "\n";
	signbuf += content_md5;
	signbuf += "\n";
	signbuf += content_type;
	signbuf += "\n";
	signbuf += date;
	signbuf += "\n"; // date
	StringMap::iterator it = _amz_data.begin();
	for (; it != _amz_data.end(); ++ it)
	{
		signbuf += it->first;
		signbuf += ":";
		signbuf += it->second;
		signbuf += "\n";
	}
	signbuf += resource;

	// Generate an HMAC-SHA-1 of the signbuf
	HMAC_SHA1(hmac, (unsigned char *) _s_secret_key.c_str(),
		_s_secret_key.length(),
		(unsigned char *) signbuf.c_str(), signbuf.length());

	// Now base-64 encode the results
	int b64Len = base64Encode(hmac, 20, b64);

	char auth_val_buf[128];
	sprintf_s(auth_val_buf, 128, "SINA %s:%.*s", 
		_s_access_key.c_str(), 10, &b64[5]);

	return auth_val_buf;
}

void TaskBase::Run()
{
	assert(_running == FALSE);
	_running = TRUE;

	char date[64];
	time_t now = time(NULL) + S3_EXPIRES_TIMEOFFSET;
	strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&now));
	add_header("Date", date);
	_date = date;

	RunImp();

	std::string auth_code = compose_auth_header(http_method(), _amz_data, uri(), _date, _content_type);
	add_header("Authorization", auth_code);

	singleton_ex<multi_http>::get_instance()->add_request(this);
}

void TaskBase::handle_curl_error( int error_code )
{
	assert(error_code != 0);
	AfxGetApp()->GetMainWnd()->PostMessage(WM_BASE+_type, (WPARAM)error_code);
}

void TaskBase::Cancel()
{
	if (_running)
	{
		singleton_ex<multi_http>::get_instance()->del_request(this);
	}
}

//////////////////////////////////////////////////////////////////////////
// list server
void ListServiceTask::handle_curl_completed( const std::string& response, long status )
{
	ListServiceResult* ret = new ListServiceResult;
	ret->status = status;

	std::string display_name = "";
	std::string id = "";

	if (status == 200)
	{
		try
		{
			Json::Value json_root;
			Json::Value json_value;
			Json::Reader json_reader;

			if (json_reader.parse(response.c_str(), json_root))
			{
				json_value = json_root["Owner"];
				if (!json_value.empty())
				{
					display_name = json_value["DisplayName"].asString();
					id = json_value["ID"].asString();
				}

				json_value = json_root["Buckets"];
				if (!json_value.empty())
				{
					for (unsigned int i = 0; i < json_value.size(); ++ i)
					{
						unsigned long long consumed_bytes = json_value[i]["ConsumedBytes"].asUInt();
						std::string creation_date = json_value[i]["CreationDate"].asString();
						std::string name = json_value[i]["Name"].asString();

						ListServiceItem bucket_item;
						bucket_item.bytes = consumed_bytes;
						bucket_item.name = string2CString(Utf8ToMultiByte(name));
						bucket_item.date = string2CString(creation_date);

						ret->items.push_back(bucket_item);
					}
				}
			}
		}
		catch (...)
		{
			ret->status = 999;
		}
	}

	ret->display_name = string2CString(display_name);
	ret->id = string2CString(id);

	AfxGetApp()->GetMainWnd()->PostMessage(WM_MAINDLG_ListServiceResult, (WPARAM)0, (LPARAM)ret);
}

//////////////////////////////////////////////////////////////////////////
// list bucket
void ListBucketTask::RunImp()
{
	std::string prefix = CString2string(_prefix);
	prefix = MultiByteToUtf8(prefix);
	add_params("prefix", URLEncode(prefix));
	add_params("delimiter", URLEncode("/"));

	std::string uri_str = uri();
	std::string buckname = CString2string(_buckname);
	buckname = MultiByteToUtf8(buckname);
	set_uri(uri_str.append(URLEncode(buckname) + "/"));
}

void ListBucketTask::handle_curl_completed( const std::string& response, long status )
{
	ListBucketResult* ret = new ListBucketResult;
	ret->status = status;

	if (status == 200)
	{
		try
		{
			Json::Value json_root;
			Json::Value json_value;
			Json::Reader json_reader;

			if (json_reader.parse(response.c_str(), json_root))
			{
				json_value = json_root["CommonPrefixes"];
				if (!json_value.empty())
				{
					for (unsigned int i = 0; i < json_value.size(); ++ i)
					{
						std::string name = json_value[i]["Prefix"].asString();

						ListBucketItem item;
						item.is_dir = true;
						item.name = string2CString(Utf8ToMultiByte(name));
						ret->items.push_back(item);
					}
				}

				json_value = json_root["Contents"];
				if (!json_value.empty())
				{
					for (unsigned int i = 0; i < json_value.size(); ++ i)
					{
						std::string name = json_value[i]["Name"].asString();
						std::string sha1 = json_value[i]["SHA1"].asString();
						std::string md5 = json_value[i]["MD5"].asString();
						std::string date = json_value[i]["Last-Modified"].asString();
						std::string owner = json_value[i]["Owner"].asString();
						unsigned long long size = json_value[i]["Size"].asUInt();

						ListBucketItem item;
						item.is_dir = false;
						item.name = string2CString(Utf8ToMultiByte(name));
						item.sha1 = string2CString(sha1);
						item.md5 = string2CString(md5);
						item.date = string2CString(date);
						item.owner = string2CString(owner);
						item.size = size;
						ret->items.push_back(item);
					}
				}
			}
		}
		catch (...)
		{
			ret->status = 999;
		}
	}

	ret->buckname = _buckname;
	ret->foldername = _prefix;

	AfxGetApp()->GetMainWnd()->PostMessage(WM_MAINDLG_ListBucketResult, (WPARAM)0, (LPARAM)ret);
}

//////////////////////////////////////////////////////////////////////////
// crate bucket
void CreateBucketTask::RunImp()
{
	set_is_upload(true);
	set_http_method(HTTP_METHOD_PUT);
	std::string uri_str = uri();
	std::string buckname = CString2string(_buckname);
	buckname = MultiByteToUtf8(buckname);
	set_uri(uri_str.append(URLEncode(buckname) + "/"));
}

void CreateBucketTask::handle_curl_completed( const std::string& response, long status )
{
	AfxGetApp()->GetMainWnd()->PostMessage(WM_MAINDLG_CreateBucketResult, (WPARAM)0, (LPARAM)status);
}

//////////////////////////////////////////////////////////////////////////
// delete bucket
void DeleteBucketTask::RunImp()
{
	set_http_method(HTTP_METHOD_DELETE);
	std::string uri_str = uri();
	std::string buckname = CString2string(_buckname);
	buckname = MultiByteToUtf8(buckname);
	set_uri(uri_str.append(URLEncode(buckname) + "/"));
}

void DeleteBucketTask::handle_curl_completed( const std::string& response, long status )
{
	AfxGetApp()->GetMainWnd()->PostMessage(WM_MAINDLG_DeleteBucketResult, (WPARAM)0, (LPARAM)status);
}

//////////////////////////////////////////////////////////////////////////
// create folder
void CreateFolderTask::RunImp()
{
	set_is_upload(true);
	set_http_method(HTTP_METHOD_PUT);
	std::string uri_str = uri();
	std::string buckname = CString2string(_buckname);
	buckname = MultiByteToUtf8(buckname);
	std::string foldername = CString2string(_foldername);
	foldername = MultiByteToUtf8(foldername);
	set_uri(uri_str.append(URLEncode(buckname) + "/" + URLEncode(foldername)));
}

void CreateFolderTask::handle_curl_completed( const std::string& response, long status )
{
	AfxGetApp()->GetMainWnd()->PostMessage(WM_MAINDLG_CreateFolderResult, (WPARAM)0, (LPARAM)status);
}

//////////////////////////////////////////////////////////////////////////
// delete object
void DeleteObjectTask::RunImp()
{
	set_http_method(HTTP_METHOD_DELETE);
	std::string uri_str = uri();
	std::string buckname = CString2string(_buckname);
	buckname = MultiByteToUtf8(buckname);
	std::string objname = CString2string(_objname);
	objname = MultiByteToUtf8(objname);
	set_uri(uri_str.append(URLEncode(buckname) + "/" + URLEncode(objname)));
}

void DeleteObjectTask::handle_curl_completed( const std::string& response, long status )
{
	AfxGetApp()->GetMainWnd()->PostMessage(WM_MAINDLG_DeleteObjectResult, (WPARAM)0, (LPARAM)status);
}

//////////////////////////////////////////////////////////////////////////
// upload
void UploadTask::RunImp()
{
	set_timeout(-1);
	set_is_upload(true);
	set_http_method(HTTP_METHOD_PUT);

	std::string uri_str = uri();
	std::string buckname = CString2string(_buckname);
	buckname = MultiByteToUtf8(buckname);
	std::string folderfile = CString2string(_filename);
	folderfile = MultiByteToUtf8(folderfile);
	set_uri(uri_str.append(URLEncode(buckname) + "/" + URLEncode(folderfile)));

	set_file_name(CString2string(_localpath));
	set_file_begin_pos(0);
	set_file_size(-1);
}

void UploadTask::handle_curl_completed( const std::string& response, long status )
{
	UploadResult* ret = new UploadResult;
	ret->status = status;
	ret->id = _id;
	AfxGetApp()->GetMainWnd()->PostMessage(WM_MAINDLG_UploadResult, (WPARAM)0, (LPARAM)ret);
}

void UploadTask::handle_trans_bytes( uint64 trans_bytes, uint64 total )
{
	_speed_calc.add_bytes(trans_bytes - _trans_total);
	_trans_total = trans_bytes;
}

//////////////////////////////////////////////////////////////////////////
// download
void DownloadTask::RunImp()
{
	set_timeout(-1);
	set_is_download(true);

	std::string uri_str = uri();
	std::string buckname = CString2string(_buckname);
	buckname = MultiByteToUtf8(buckname);
	std::string folderfile = CString2string(_filename);
	folderfile = MultiByteToUtf8(folderfile);
	set_uri(uri_str.append(URLEncode(buckname) + "/" + URLEncode(folderfile)));
	set_file_name(CString2string(_localpath + _T(DOWNLOAD_TEMP)));
}

void DownloadTask::handle_curl_completed( const std::string& response, long status )
{
	CString from = _localpath;
	from += _T(DOWNLOAD_TEMP);
	MoveFileEx(from, _localpath, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING);
	
	DownloadResult* ret = new DownloadResult;
	ret->status = status;
	ret->id = _id;
	AfxGetApp()->GetMainWnd()->PostMessage(WM_MAINDLG_DownloadResult, (WPARAM)0, (LPARAM)ret);
}

void DownloadTask::handle_trans_bytes( uint64 trans_bytes, uint64 total )
{
	set_file_size(total);
	_speed_calc.add_bytes(trans_bytes - _trans_total);
	_trans_total = trans_bytes;
}

//////////////////////////////////////////////////////////////////////////
// meta
void MetaTask::RunImp()
{
	std::string uri_str = uri();
	std::string buckname = CString2string(_buckname);
	buckname = MultiByteToUtf8(buckname);
	set_uri(uri_str.append(URLEncode(buckname) + "/?meta"));
}

void MetaTask::handle_curl_completed( const std::string& response, long status )
{
	char* attr[] = 
	{
		"DeleteQuantity", 
		"Capacity", 
		"PoolName", 
		"ProjectID", 
		"DownloadQuantity", 
		"DownloadCapacity", 
		"RelaxUpload", 
		"CapacityC", 
		"QuantityC", 
		"Project", 
		"UploadCapacity", 
		"UploadQuantity", 
		"Last-Modified", 
		"SizeC", 
		"Owner", 
		"DeleteCapacity", 
		"Quantity"
	};

	char* pow[] = {"read", "write", "read_acp", "write_acp"};

	std::map<std::string, char> pow_mark_map;
	for (int i = 0; i < ARRAY_SIZE(pow); ++ i)
	{
		pow_mark_map.insert(std::map<std::string, char>::
			value_type(pow[i], 1 << i));
	}

	MetaResult* ret = new MetaResult;
	ret->status = status;
	ret->buckname = _buckname;
	if (status == 200)
	{
		try
		{
			Json::Value json_root;
			Json::Value json_value;
			Json::Reader json_reader;

			if (json_reader.parse(response.c_str(), json_root))
			{
				if (!json_root.empty())
				{
					for (int i = 0; i < ARRAY_SIZE(attr); ++ i)
					{
						char* key = attr[i];
						std::string val = api_helper::asString(json_root[key]);
						ret->results.insert(make_pair(key, val));
					}

					json_value = json_root["ACL"];
					if (!json_value.empty())
					{
						Json::Value::Members mem_names = json_value.getMemberNames();
						Json::Value::Members::iterator it_name = mem_names.begin();
						for (; it_name != mem_names.end(); ++ it_name)
						{
							std::string key = *it_name;
							char pow_mark = 0;
							Json::Value acl_value = json_value[key.c_str()];
							if (!acl_value.empty())
							{
								for (unsigned int j = 0; j < acl_value.size(); ++ j)
								{
									std::string str = acl_value[j].asString();
									std::map<std::string, char>::iterator it = 
										pow_mark_map.find(str);
									if (it != pow_mark_map.end())
									{
										pow_mark |= it->second;
									}
								}
								ret->acls.insert(make_pair(key, pow_mark));
							}
						}
					}
				}
			}
		}
		catch (...)
		{
			ret->status = 999;
		}
	}
	AfxGetApp()->GetMainWnd()->PostMessage(WM_MAINDLG_MetaResult, (WPARAM)0, (LPARAM)ret);
}

//////////////////////////////////////////////////////////////////////////
// acl
AclTask::AclTask( const CString& buckname, 
				 const CString& objname, 
				 std::map<CString, char>& acls ) 
	: _buckname(buckname)
	, _objname(objname)
{
	_type = kAcl;

	char* pow[] = {"read", "write", "read_acp", "write_acp"};

	Json::Value json_root;
	std::map<CString, char>::iterator it = acls.begin();
	for (; it != acls.end(); ++ it)
	{
		std::string name = CString2string(it->first);
		char acl = it->second;
		for (int i = 0; i < ACL_COUNT; ++ i)
		{
			if (acl & 0x1)
			{
				json_root[name.c_str()].append(pow[i]);
			}
			acl >>= 1;
		}
	}

	Json::FastWriter json_writer;
	std::string body_str = json_writer.write(json_root);
	set_body(body_str);
}


void AclTask::RunImp()
{
	set_http_method(HTTP_METHOD_PUT);
	_content_type = "text/json";
	add_header("Content-Type", _content_type);
	std::string uri_str = uri();
	std::string buckname = CString2string(_buckname);
	buckname = MultiByteToUtf8(buckname);
	std::string objname = CString2string(_objname);
	objname = MultiByteToUtf8(objname);
	set_uri(uri_str.append(URLEncode(buckname) + "/" + URLEncode(objname) + "?acl"));
}

void AclTask::handle_curl_completed( const std::string& response, long status )
{
	AfxGetApp()->GetMainWnd()->PostMessage(WM_MAINDLG_AclResult, (WPARAM)0, (LPARAM)status);
}

//////////////////////////////////////////////////////////////////////////
// get acl
GetAclTask::GetAclTask( const CString& buckname, 
					   const CString& objname, 
					   ListBucketItem* pItem ) 
					   : _buckname(buckname)
					   , _objname(objname)
{
	_type = kGetAcl;
	if (pItem != NULL)
	{
		_item = *pItem;
	}
}

void GetAclTask::RunImp()
{
	std::string uri_str = uri();
	std::string buckname = CString2string(_buckname);
	buckname = MultiByteToUtf8(buckname);
	std::string objname = CString2string(_objname);
	objname = MultiByteToUtf8(objname);
	set_uri(uri_str.append(URLEncode(buckname) + "/" + URLEncode(objname) + "?acl"));
}

void GetAclTask::handle_curl_completed( const std::string& response, long status )
{
	GetAclResult* ret = new GetAclResult;
	ret->status = status;
	ret->buckname = _buckname;
	ret->objname = _objname;
	ret->item = _item;

	char* pow[] = {"read", "write", "read_acp", "write_acp"};

	std::map<std::string, char> pow_mark_map;
	for (int i = 0; i < ARRAY_SIZE(pow); ++ i)
	{
		pow_mark_map.insert(std::map<std::string, char>::
			value_type(pow[i], 1 << i));
	}

	if (status == 200)
	{
		try
		{
			Json::Value json_root;
			Json::Value json_value;
			Json::Reader json_reader;

			if (json_reader.parse(response.c_str(), json_root))
			{
				if (!json_root.empty())
				{
					std::string owner = api_helper::asString(json_root["Owner"]);
					ret->owner = string2CString(owner);

					json_value = json_root["ACL"];
					if (!json_value.empty())
					{
						Json::Value::Members mem_names = json_value.getMemberNames();
						Json::Value::Members::iterator it_name = mem_names.begin();
						for (; it_name != mem_names.end(); ++ it_name)
						{
							std::string key = *it_name;
							char pow_mark = 0;
							Json::Value acl_value = json_value[key.c_str()];
							if (!acl_value.empty())
							{
								for (unsigned int j = 0; j < acl_value.size(); ++ j)
								{
									std::string str = acl_value[j].asString();
									std::map<std::string, char>::iterator it = 
										pow_mark_map.find(str);
									if (it != pow_mark_map.end())
									{
										pow_mark |= it->second;
									}
								}
								ret->acls.insert(make_pair(key, pow_mark));
							}
						}
					}
				}
			}
		}
		catch (...)
		{
			ret->status = 999;
		}
	}

	AfxGetApp()->GetMainWnd()->PostMessage(WM_MAINDLG_GetAclResult, (WPARAM)0, (LPARAM)ret);
}

//////////////////////////////////////////////////////////////////////////
// copy
void CopyTask::RunImp()
{
	set_is_upload(true);
	set_http_method(HTTP_METHOD_PUT);
	std::string buckname = CString2string(_buckname);
	buckname = MultiByteToUtf8(buckname);

	std::string source = "/";
	source.append(buckname + "/" + CString2string(_frompath));
	source = MultiByteToUtf8(source);
	add_header("x-amz-copy-source", URLEncode(source));
	add_header("x-amz-metadata-directive", "COPY");

	_amz_data.insert(StringMap::value_type("x-amz-copy-source", URLEncode(source)));
	_amz_data.insert(StringMap::value_type("x-amz-metadata-directive", "COPY"));

	std::string uri_str = uri();
	
	std::string objname = CString2string(_topath);
	objname = MultiByteToUtf8(objname);
	set_uri(uri_str.append(URLEncode(buckname) + "/" + URLEncode(objname)));
}

void CopyTask::handle_curl_completed( const std::string& response, long status )
{
	AfxGetApp()->GetMainWnd()->PostMessage(WM_MAINDLG_CopyResult, (WPARAM)0, (LPARAM)status);
}

//////////////////////////////////////////////////////////////////////////
// rename
void RenameTask::handle_curl_completed( const std::string& response, long status )
{
	RenameResult* ret = new RenameResult;
	ret->status = status;
	ret->buckname = _buckname;
	ret->fromname = _frompath;
	ret->toname = _topath;
	AfxGetApp()->GetMainWnd()->PostMessage(WM_MAINDLG_RenameResult, (WPARAM)0, (LPARAM)ret);
}
