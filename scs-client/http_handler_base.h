#ifndef _HTTP_HANDLER_BASE_
#define _HTTP_HANDLER_BASE_

#include <string>
#include <map>

typedef int                 int32;
typedef long long           int64;

typedef unsigned int       uint32;
typedef unsigned long long uint64;


#define HTTP_METHOD_GET				"GET"
#define HTTP_METHOD_POST			"POST"
#define HTTP_METHOD_PUT				"PUT"
#define HTTP_METHOD_DELETE			"DELETE"



class http_handler_base
{
#define CPP_SYNTHESIZE(varType, varName, funName)\
protected: varType varName;\
public: varType funName(void) const { return varName; }\
public: void set_##funName(varType var){ varName = var; }

#define CPP_SYNTHESIZE_BY_REF(varType, varName, funName)\
protected: varType varName;\
public: const varType& funName(void) { return varName; }\
public: void set_##funName(const varType& var) { varName = var; }

public:
	typedef std::map<std::string, std::string> map_params;

public:
	http_handler_base()
		: _is_pending(false)
		, _use_https(false)
		, _is_upload(false)
		, _is_download(false)
		, _host("")
		, _uri("")
		, _http_method(HTTP_METHOD_GET)
		, _file_name("")
		, _file_size(0)
		, _file_begin_pos(0)
		, _body("")
		, _timeout(-1)
	{}
	virtual ~http_handler_base() 
	{
		_head_params.clear();
		_content_params.clear();
	}

	CPP_SYNTHESIZE(bool, _is_pending, is_pending);
	CPP_SYNTHESIZE(bool, _use_https, use_https);
	CPP_SYNTHESIZE(bool, _is_upload, is_upload);
	CPP_SYNTHESIZE(bool, _is_download, is_download);
	CPP_SYNTHESIZE_BY_REF(std::string, _host, host);
	CPP_SYNTHESIZE_BY_REF(std::string, _uri, uri);
	CPP_SYNTHESIZE_BY_REF(std::string, _http_method, http_method);
	CPP_SYNTHESIZE_BY_REF(std::string, _file_name, file_name);
	CPP_SYNTHESIZE_BY_REF(std::string, _body, body);
	CPP_SYNTHESIZE(uint64, _file_size, file_size);
	CPP_SYNTHESIZE(uint64, _file_begin_pos, file_begin_pos);
	CPP_SYNTHESIZE(uint32, _timeout, timeout);

	const map_params& get_header_params() { return _head_params; }
	const map_params& get_content_params() { return _content_params; }
	void add_header(const std::string key, const std::string val)
	{
		_head_params.insert(make_pair(key, val));
	}
	void add_params(const std::string key, const std::string val)
	{
		_content_params.insert(make_pair(key, val));
	}

public:
	virtual void handle_curl_completed(const std::string& response, long status) = 0;
	virtual void handle_curl_error(int error_code) = 0;
	virtual void handle_response_header(const std::string& resp_header) {}
	virtual void handle_trans_bytes(uint64 trans_bytes, uint64 total) {}

protected:
	map_params			_head_params;
	map_params			_content_params;
};

#endif