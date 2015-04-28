#ifndef _HTTP_REQUEST_
#define _HTTP_REQUEST_

#include <map>
#include <assert.h>
#include <fstream>
#include "http_handler_base.h"
#include "curl/curl.h"


typedef enum enumRequestType
{
	RequestTypeCommand,
	RequestTypeUpload,
	RequestTypeDownload,
} RequestType;

typedef enum enumRequestState
{
	RequestStateIdle,
	RequestStateSend,
	RequestStateSendCompleted,
	RequestStateRecv,
	RequestStateRecvCompleted,
} RequestState;

class multi_http;
struct http_request
{
	http_request(multi_http* curlm)
		: _curlm(curlm)
	{
		_s_http_request_count ++;
		_ref_count = 1;
		_handler = NULL;
		_curl = NULL;
		_rsp_str = "";
		_type = RequestTypeCommand;
		_state = RequestStateIdle;
		_local_file_path.clear();
		_file_size = 0;
		_file_begin_pos = 0;
		_trans_bytes = 0;
		_url_str = "";
		_http_mothod = "";
		_post_fields = "";
		_content_length = 0;
		_status_code = 0;
		_headerlist = NULL;
		_multipart_content = false;
		_body_str = "";
		_timeout = -1;
	}

	~http_request() 
	{
		_s_http_request_count --;
		if (_file_handle.is_open())
		{
			_file_handle.close();
		}
	}

	void add_ref()
	{
		InterlockedIncrement(&_ref_count);
	}

	void release_ref()
	{
		if(!InterlockedDecrement(&_ref_count))
		{
			assert(_handler == NULL);
			delete this;
		}
	}

	long ref()
	{
		return _ref_count;
	}

	void be_useless() { _handler = NULL; }

	long						_ref_count;
	multi_http*					_curlm;
	http_handler_base*			_handler;
	CURL*						_curl;

	std::string					_http_mothod;
	std::string					_url_str;
	std::string					_post_fields;
	struct curl_slist*			_headerlist;
	std::string					_rsp_str;
	std::string					_header_str;
	std::string					_body_str;
	bool						_multipart_content;
	RequestType					_type;
	RequestState				_state;
	uint32						_timeout;

	std::string					_local_file_path;
	std::fstream				_file_handle;
	uint64						_file_size;
	uint64						_file_begin_pos;

	long						_status_code;
	double						_content_length;
	uint64						_trans_bytes;
	
	http_handler_base::map_params _header_params;
	http_handler_base::map_params _content_params;

	static long					_s_http_request_count;
};

#endif