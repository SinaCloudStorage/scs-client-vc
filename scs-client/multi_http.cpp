#include "multi_http.h"
#include <assert.h>
#include <sys/stat.h>

#ifdef WIN32
#	ifndef MH_STST64
#		define MH_STST64 _stat64
#	endif
#	ifndef MH_SLEEP
#		define MH_SLEEP(x) Sleep(1000)
#	endif
#else
#	ifndef MH_STST64
#		define MH_STST64 _stat
#	endif
#	ifndef MH_SLEEP
#		define MH_SLEEP(x) nsleep(x*1000)
#	endif
#endif

#define MSG_CURL_TRANS_COMPLETED			0
#define MSG_CURL_TRANS_INFO					1
#define MSG_CULR_CONTENT_LENGTH				2
#define MSG_CURL_RESPONSE_HEADER			3
#define MSG_CURL_FILE_IO_ERROR				4
#define MSG_CULR_TRANS_ERROR				6


long http_request::_s_http_request_count = 0;

#ifdef RECORD_LOG
static size_t curl_debug(CURL* pcurl, curl_infotype type, char* data, size_t size, void* stream)
{
	std::string str = "";
	http_request* req_ptr = (http_request*)stream;

	if (req_ptr->_handler == NULL)
		return 0;

	switch (type)
	{
	case CURLINFO_HEADER_OUT:
		{
			str.append(data, size);

			LOG4CPLUS_DEBUG( multi_http::s_logger, "[HEADER OUT]: \n" << str.c_str() );

			if ( req_ptr->_http_mothod == HTTP_METHOD_GET || 
				(req_ptr->_http_mothod == HTTP_METHOD_POST && req_ptr->_content_params.empty()) )
			{
				req_ptr->_state = RequestStateSendCompleted;
			}
		}
		break;

	case CURLINFO_DATA_OUT:
		{
			if (req_ptr->_type != RequestTypeUpload)
			{
				str.append(data, size);
				LOG4CPLUS_DEBUG( multi_http::s_logger, "[DATA OUT]: \n" << str.c_str() );
			}

			if (req_ptr->_file_size == req_ptr->_trans_bytes)
			{
				req_ptr->_state = RequestStateSendCompleted;
			}
		}
		break;

	case CURLINFO_HEADER_IN:
		{
			if (req_ptr->_state != RequestStateRecv)
			{
				req_ptr->_state = RequestStateRecv;
			}
		}
		break;

	case CURLINFO_DATA_IN:
		{
			if (req_ptr->_type != RequestTypeDownload)
			{
				str.append(data, size);
				LOG4CPLUS_DEBUG( multi_http::s_logger, "[DATA IN]: \n" << str.c_str() );
			}
		}
		break;
	}
	return 0;
}
#endif // RECORD_LOG

static size_t header_callback(char *buffer, size_t size, size_t nmemb, void* stream)    
{
	http_request* req_ptr = (http_request*)stream;
	size_t len  = size * nmemb;  
	req_ptr->_header_str.append(buffer, len);

	if (0 == memcmp(buffer, "\r\n", sizeof("\r\n")))
	{
		curl_easy_getinfo(req_ptr->_curl, 
			CURLINFO_CONTENT_LENGTH_DOWNLOAD, 
			&req_ptr->_content_length);

		if (req_ptr->_curlm)
		{
			req_ptr->_curlm->post_response_header(req_ptr);
		}
	}
	return len;  
}

static size_t read_callback(char *buffer, size_t count, size_t size, void* stream)
{
	http_request* req_ptr = (http_request*)stream;
	if (req_ptr->_handler == NULL)
		return 0;

	uint64 read_byets = 0;
	if (!req_ptr->_body_str.empty())
	{
		uint32 len = req_ptr->_body_str.size();
		uint32 last = len - req_ptr->_trans_bytes;
		uint64 can_read_bytes = std::min<uint64>(count * size, last);
		memcpy(buffer, req_ptr->_body_str.c_str() + req_ptr->_trans_bytes, can_read_bytes);
		req_ptr->_trans_bytes += can_read_bytes;
		read_byets = can_read_bytes;
	}
	else
	{
		std::fstream& filestream = req_ptr->_file_handle;
		uint64 begin_pos = req_ptr->_file_begin_pos + req_ptr->_trans_bytes;
		uint64 read_size = req_ptr->_file_size - req_ptr->_trans_bytes;

		if (!filestream.is_open())
		{
			filestream.open(req_ptr->_local_file_path, 
				std::ios::in | std::ios::out | std::ios::binary);
			if (!filestream.is_open())
			{
				return -1; // file not exist!
			}
			filestream.seekg(begin_pos, std::ios::beg);
		}

		assert((uint64)filestream.tellg() == begin_pos);
		uint64 can_read_bytes = std::min<uint64>(count * size, read_size);
		filestream.read(buffer, can_read_bytes);
		read_byets = filestream.gcount();
		req_ptr->_trans_bytes += read_byets;
	}

	LOG4CPLUS_DEBUG( multi_http::s_logger, "ReadFile: byte: " << read_byets
		<< "total byte: " << req_ptr->_trans_bytes);

	return read_byets;
}

static size_t write_callback(char *buffer, size_t count, size_t size, void* stream)
{
	http_request* req_ptr = (http_request*)stream;
	if (req_ptr->_handler == NULL)
		return 0;

	if (req_ptr->_status_code == 0)
	{
		curl_easy_getinfo(req_ptr->_curl, CURLINFO_RESPONSE_CODE, &req_ptr->_status_code);
	}

	if (req_ptr->_type == RequestTypeDownload && 
		(req_ptr->_status_code / 100) == 2)
	{
		std::fstream& filestream = req_ptr->_file_handle;
		uint64 begin_pos = req_ptr->_file_begin_pos + req_ptr->_trans_bytes;

		if (!filestream.is_open())
		{
			filestream.open(req_ptr->_local_file_path, 
				std::ios::out | std::ios::binary);
			if (!filestream.is_open())
			{
				return -1; // open file failed!
			}
			filestream.seekp(begin_pos, std::ios::beg);
		}

		assert((uint64)filestream.tellp() == begin_pos);
		uint64 can_write_bytes = count * size;
		filestream.write(buffer, can_write_bytes);
		filestream.flush();
		req_ptr->_trans_bytes += can_write_bytes;
	}
	else
	{
		std::string* pStream = static_cast<std::string*>(&req_ptr->_rsp_str);
		req_ptr->_trans_bytes = count * size;
		if (req_ptr->_trans_bytes >= req_ptr->_content_length)
		{
			req_ptr->_state = RequestStateRecvCompleted;
		}
		pStream->append((char*)buffer, count * size);
	}

	return count * size;
}

LOG_CLASS_IMPLEMENT(multi_http, TEXT("multi_http"));

multi_http::multi_http()
	: _msg_sender(NULL)
{
	start();
}

multi_http::~multi_http()
{
	stop();
	while (is_running())
	{
		MH_SLEEP(500);
	}
//	assert(http_request::_s_http_request_count == 0);
}

void multi_http::run()
{
	LOG_DEBUG("run multi_http thread...");

	int	msg_count = -1;
	int still_running = 0;
	DWORD wait_timeout = -1;

	curl_global_init(CURL_GLOBAL_ALL);
	_curlm = curl_multi_init();
	curl_multi_setopt(_curlm, CURLMOPT_MAXCONNECTS, 100);

	while (1)
	{
		remove_deleted_request();
		if (_add_event.wait(wait_timeout) != 0)
		{// timeout
			collect_transmit_info();
		}
		run_waiting_request();

		still_running = multi_perform();
		if (still_running == -1)
		{
			break;
		}

		while ((_msg = curl_multi_info_read(_curlm, &msg_count)))
		{
			if (_msg->msg == CURLMSG_DONE)
			{
				CURL* e = _msg->easy_handle;
				handle_completed(e, _msg->data.result);
			}
			else
			{
				LOG_ERROR("curlmsg error: " << _msg->msg);
			}
		}

		wait_timeout = (still_running == 0) ? 500 : 0;
	}

	cleanup();
}

void multi_http::cleanup()
{
	clear_all_requests();
	curl_multi_cleanup(_curlm);
	curl_global_cleanup();
}

int multi_http::multi_perform()
{
	fd_set fdread, fdwrite, fdexcep;
	int still_running = 0;
	long curl_timeo = -1;
	int max_fd = -1;
	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	curl_multi_perform(_curlm, &still_running);
	if (still_running)
	{
		FD_ZERO(&fdread);
		FD_ZERO(&fdwrite);
		FD_ZERO(&fdexcep);

		if (curl_multi_timeout(_curlm, &curl_timeo))
		{
			LOG_ERROR("curl_multi_timeout, error.");
			return -1;
		}
		if (curl_timeo >= 0)
		{
			timeout.tv_sec = curl_timeo / 1000;
			if (timeout.tv_sec > 1)
				timeout.tv_sec = 1;
			else
				timeout.tv_usec = (curl_timeo % 1000) * 1000;
		}

		if (curl_multi_fdset(_curlm, &fdread, &fdwrite, &fdexcep, &max_fd) ||
			max_fd == -1)
		{
			LOG_ERROR("curl_multi_fdset error.");
			return -1;
		}

		int rc = select(max_fd + 1, &fdread, &fdwrite, &fdexcep, &timeout);
		switch (rc)
		{
		case -1: // select error
			break;

		case 0: // select timeout
			break;

		default: // one or more of curl's file descriptors say there's data to read or write
			break;
		}
	}
	return still_running;
}

void multi_http::add_request( http_handler_base* handler_ptr )
{
	handler_ptr->set_is_pending(true);

	{
		scoped_lock locker(_request_list_mutex);

		do_delete_request(handler_ptr);
		http_request* request = create_request(handler_ptr);
		_wait_add_requests.push_back(request);
		_add_event.set();
	}
}

void multi_http::del_request( http_handler_base* handler_ptr )
{
	handler_ptr->set_is_pending(false);

	{
		scoped_lock locker(_request_list_mutex);

		do_delete_request(handler_ptr);
	}
}

http_request* multi_http::create_request( http_handler_base* handler_ptr )
{
	assert(handler_ptr);

	http_request* request = new http_request(this);
	request->_handler = handler_ptr;

	request->_http_mothod = handler_ptr->http_method();
	request->_url_str += handler_ptr->use_https() ? "https://" : "http://";
	request->_url_str += handler_ptr->host();
	request->_url_str += handler_ptr->uri();
	request->_timeout = handler_ptr->timeout();

	const http_handler_base::map_params& header_params = 
		handler_ptr->get_header_params();
	http_handler_base::map_params::const_iterator it_header = 
		header_params.begin();
	for (; it_header != header_params.end(); ++ it_header)
	{
		request->_header_params.insert(
			make_pair(it_header->first, it_header->second));
	}

	const http_handler_base::map_params& content_params = 
		handler_ptr->get_content_params();
	http_handler_base::map_params::const_iterator it_content = 
		content_params.begin();
	for (; it_content != content_params.end(); ++ it_content)
	{
		request->_content_params.insert(
			make_pair(it_content->first, it_content->second));
	}

	// file info
	request->_local_file_path = handler_ptr->file_name();
	request->_file_begin_pos = handler_ptr->file_begin_pos();
	request->_file_size = handler_ptr->file_size();
	request->_body_str = handler_ptr->body();
	if (handler_ptr->is_upload())
	{
		request->_type = RequestTypeUpload;
		modify_upload_file_size(request, handler_ptr);
	}
	else if (handler_ptr->is_download())
	{
		request->_type = RequestTypeDownload;
	}

	return request;
}

void multi_http::do_delete_request( http_handler_base* handler_ptr )
{
	RequestList::iterator it_wait = _wait_add_requests.begin();
	for (; it_wait != _wait_add_requests.end(); ++ it_wait)
	{
		if ((*it_wait)->_handler == handler_ptr)
		{
			http_request* req_ptr = *it_wait;
			req_ptr->be_useless();
			_wait_add_requests.erase(it_wait);
			req_ptr->release_ref();
			return;
		}
	}

	RequestList::iterator it_run = _running_requests.begin();
	for (; it_run != _running_requests.end(); ++ it_run)
	{
		http_request* req_ptr = *it_run;
		if (req_ptr->_handler == handler_ptr)
		{
			req_ptr->be_useless();
			_wait_del_requests.push_back(req_ptr);
			_running_requests.erase(it_run);
			return;
		}
	}
}

void multi_http::remove_deleted_request()
{
	scoped_lock locker(_request_list_mutex);

	// remove running handler, delete request.
	RequestList::iterator it_del = _wait_del_requests.begin();
	for (; it_del != _wait_del_requests.end(); )
	{
		http_request* req_ptr = *it_del;

		curl_multi_remove_handle(_curlm, req_ptr->_curl);
		curl_easy_cleanup(req_ptr->_curl);
		curl_slist_free_all(req_ptr->_headerlist);
		req_ptr->_curlm = NULL;
		req_ptr->release_ref();
		
		it_del = _wait_del_requests.erase(it_del);
	}
}

void multi_http::run_waiting_request()
{
	scoped_lock locker(_request_list_mutex);

	RequestList::iterator it = _wait_add_requests.begin();
	for (; it != _wait_add_requests.end(); ++ it)
	{
		http_request* req_ptr = *it;
		build_request(req_ptr);
		curl_multi_add_handle(_curlm, req_ptr->_curl);
		req_ptr->_state = RequestStateSend;
		_running_requests.push_back(*it);
	}
	_wait_add_requests.clear();
}

void multi_http::build_http_parameters( CURL* eh, http_request* req_ptr )
{
	if (req_ptr->_multipart_content)
	{
		struct curl_httppost* formpost = NULL;
		struct curl_httppost* lastptr = NULL;

		assert(req_ptr->_http_mothod == HTTP_METHOD_POST);
		http_handler_base::map_params::iterator it_cont = req_ptr->_content_params.begin();
		for (; it_cont != req_ptr->_content_params.end(); ++ it_cont)
		{
			curl_formadd(&formpost,
				&lastptr,
				CURLFORM_COPYNAME, it_cont->first.c_str(),
				CURLFORM_COPYCONTENTS, it_cont->second.c_str(),
				CURLFORM_END);
		}
		curl_easy_setopt(eh, CURLOPT_HTTPPOST, formpost);
	}
	else
	{
		std::string params_str = "";
		http_handler_base::map_params::iterator it_cont = req_ptr->_content_params.begin();
		for (; it_cont != req_ptr->_content_params.end(); ++ it_cont)
		{
			params_str.append(
				it_cont == req_ptr->_content_params.begin() ? 
				"" : "&");
			params_str.append(it_cont->first);
			if (!it_cont->second.empty())
			{
				params_str.append("=" + it_cont->second);
			}
		}

		if (req_ptr->_http_mothod == HTTP_METHOD_POST)
		{
			curl_easy_setopt(eh, CURLOPT_POSTFIELDS, req_ptr->_post_fields.c_str());
		}
		else
		{
			if (req_ptr->_http_mothod == HTTP_METHOD_GET)
			{
				curl_easy_setopt(eh, CURLOPT_HTTPGET, 1L);
			}
			else if (req_ptr->_http_mothod == HTTP_METHOD_PUT)
			{
				curl_easy_setopt(eh, CURLOPT_PUT, 1L);
			}
			else
			{
				curl_easy_setopt(eh, CURLOPT_CUSTOMREQUEST, req_ptr->_http_mothod.c_str());
			}

			int pos = req_ptr->_url_str.find('?');
			if (pos == -1)
			{
				req_ptr->_url_str.append("?");
			}
			else if (pos + 1 != req_ptr->_url_str.length())
			{
				req_ptr->_url_str.append("&");
			}
			req_ptr->_url_str.append(params_str);
		}
	}
}

void multi_http::build_request( http_request* req_ptr )
{
	assert(req_ptr);

	CURL* eh = curl_easy_init();
	build_http_parameters(eh, req_ptr);

	// header
	http_handler_base::map_params::iterator it_header = req_ptr->_header_params.begin();
	for (; it_header != req_ptr->_header_params.end(); ++ it_header)
	{
		std::string item = it_header->first;
		item += ": ";
		item += it_header->second;
		req_ptr->_headerlist = curl_slist_append(req_ptr->_headerlist, item.c_str());
	}

	curl_easy_setopt(eh, CURLOPT_READFUNCTION, read_callback);
	curl_easy_setopt(eh, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(eh, CURLOPT_WRITEDATA, req_ptr);
	curl_easy_setopt(eh, CURLOPT_URL, req_ptr->_url_str.c_str());
	curl_easy_setopt(eh, CURLOPT_HTTPHEADER, req_ptr->_headerlist);
// 	curl_easy_setopt(eh, CURLOPT_MAXREDIRS, 3);  //设置重定向的最大次数  
// 	curl_easy_setopt(eh, CURLOPT_FOLLOWLOCATION, 1);  //设置301、302跳转跟随location  
	curl_easy_setopt(eh, CURLOPT_HEADERFUNCTION, header_callback );  
	curl_easy_setopt(eh, CURLOPT_HEADERDATA, req_ptr);
	curl_easy_setopt(eh, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(eh, CURLOPT_SSL_VERIFYPEER, FALSE);
	curl_easy_setopt(eh, CURLOPT_SSL_VERIFYHOST, FALSE);
	curl_easy_setopt(eh, CURLOPT_TIMEOUT, req_ptr->_timeout);

#ifdef RECORD_LOG
	curl_easy_setopt(eh, CURLOPT_DEBUGFUNCTION, curl_debug);
	curl_easy_setopt(eh, CURLOPT_DEBUGDATA, req_ptr);
#endif

	// upload file operation
	if (req_ptr->_type == RequestTypeUpload)
	{
		curl_easy_setopt(eh, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(eh, CURLOPT_INFILESIZE_LARGE, 
			(curl_off_t)req_ptr->_file_size);
		curl_easy_setopt(eh, CURLOPT_READDATA, req_ptr);
	}
	else if (!req_ptr->_body_str.empty())
	{
		curl_easy_setopt(eh, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(eh, CURLOPT_INFILESIZE_LARGE, 
			(curl_off_t)req_ptr->_body_str.size());
		curl_easy_setopt(eh, CURLOPT_READDATA, req_ptr);
	}

	req_ptr->_curl = eh;
}

void multi_http::handle_completed( CURL* curl, int result )
{
	// request completed
	scoped_lock locker(_request_list_mutex);

	RequestList::iterator it = _running_requests.begin();
	for (; it != _running_requests.end(); ++ it)
	{
		http_request* req_ptr = *it;
		if (req_ptr->_curl == curl)
		{
			if (req_ptr->_status_code == 0)
			{
				curl_easy_getinfo(req_ptr->_curl, CURLINFO_RESPONSE_CODE, &req_ptr->_status_code);
			}
			_running_requests.erase(it);
			_wait_del_requests.push_back(req_ptr);

			post_complete_message(req_ptr, result);
			return;
		}
	}
}

void multi_http::post_complete_message( http_request* req_ptr, unsigned long result )
{
	assert(req_ptr);
	LOG_DEBUG( "post_complete_message : " << req_ptr );
	req_ptr->add_ref();
	post_message(MSG_CURL_TRANS_COMPLETED, (unsigned long)req_ptr, result);
}

void multi_http::post_response_header( http_request* req_ptr )
{
	assert(req_ptr);
	req_ptr->add_ref();
	post_message(MSG_CURL_RESPONSE_HEADER, (unsigned long)req_ptr);
}

void multi_http::post_transmit_info( http_request* req_ptr )
{
	assert(req_ptr);
	req_ptr->add_ref();
	post_message(MSG_CURL_TRANS_INFO, (unsigned long)req_ptr);
}

void multi_http::post_message( unsigned long msg_id, unsigned long param1 /*= 0*/, unsigned long param2 /*= 0*/ )
{
	if (_msg_sender)
	{
		_msg_sender->post_message(msg_id, param1, param2);
	}
	else
	{
		handle_message(msg_id, param1, param2);
	}
}

void multi_http::handle_message( unsigned long msg_id, unsigned long param1, unsigned long param2 )
{
	http_request* req_ptr = (http_request*)param1;
	assert(req_ptr);

	switch (msg_id)
	{
	case MSG_CURL_TRANS_COMPLETED:
		{
			LOG_DEBUG( "post_handle_message : MSG_CULR_TRANS_COMPLETED" );

			if (req_ptr->_file_handle.is_open())
			{
				req_ptr->_file_handle.close();
			}

			if (req_ptr->_handler != NULL)
			{
				req_ptr->_handler->set_is_pending(false);
				if (param2 != CURLE_OK)
				{
					req_ptr->_handler->handle_curl_error(param2);
				}
				else
				{
					req_ptr->_handler->handle_curl_completed(
						req_ptr->_rsp_str,
						req_ptr->_status_code);
				}
			}
			req_ptr->be_useless();
		}
		break;

	case MSG_CURL_RESPONSE_HEADER:
		{
			http_request* req_ptr = (http_request*)param1;
			assert(req_ptr);
			if (req_ptr->_handler != NULL)
			{
				req_ptr->_handler->handle_response_header(req_ptr->_header_str);
			}
		}
		break;

	case MSG_CURL_TRANS_INFO:
		{
			http_request* req_ptr = (http_request*)param1;
			assert(req_ptr);
			if (req_ptr->_handler != NULL)
			{
				uint64 total_bytes = 0;
				if (req_ptr->_type == RequestTypeUpload)
				{
					total_bytes = req_ptr->_file_size;
				}
				else // req_ptr->_type == RequestTypeDownload
				{
					total_bytes = req_ptr->_content_length;
				}
				req_ptr->_handler->handle_trans_bytes(req_ptr->_trans_bytes, total_bytes);
			}
		}
		break;

	default:
		break;
	}
	req_ptr->release_ref();
}

void multi_http::modify_upload_file_size( http_request* request, 
										 http_handler_base* handler_ptr )
{
	uint64 begin_pos = handler_ptr->file_begin_pos();

	struct MH_STST64 info;
	MH_STST64(request->_local_file_path.c_str(), &info);
	uint64 size = info.st_size;

	if (request->_file_size == -1 || // mean file size is unknown
		request->_file_size + begin_pos > size) // giving range is overflowed
	{
		if (begin_pos > size)
		{
			request->_file_size = 0;
		}
		else
		{
			request->_file_size = size - begin_pos;
		}
		handler_ptr->set_file_size(request->_file_size);
	}
}

void multi_http::clear_all_requests()
{
	{
		scoped_lock locker(_request_list_mutex);

		RequestList::iterator it_wait = _wait_add_requests.begin();
		for (; it_wait != _wait_add_requests.end(); ++ it_wait)
		{
			http_request* req_ptr = *it_wait;
			req_ptr->be_useless();
			req_ptr->release_ref();
		}
		_wait_add_requests.clear();

		RequestList::iterator it_run = _running_requests.begin();
		for (; it_run != _running_requests.end(); ++ it_run)
		{
			http_request* req_ptr = *it_run;
			req_ptr->be_useless();
			_wait_del_requests.push_back(req_ptr);
		}
		_running_requests.clear();
	}

	remove_deleted_request();
}

const std::string multi_http::curl_error_string( int err_code )
{
	std::string str;
	switch (err_code)
	{
	case CURLE_OK:				str = "curl ok";		break; // 0
	case CURLE_READ_ERROR:		str = "read error";		break; // 24

	default:
		str = "unknown error";
		break;
	}
	return str;
}

void multi_http::collect_transmit_info()
{
	scoped_lock locker(_request_list_mutex);

	RequestList::iterator it = _running_requests.begin();
	for (; it != _running_requests.end(); ++ it)
	{
		http_request* req_ptr = *it;
		if (req_ptr->_type == RequestTypeUpload ||
			req_ptr->_type == RequestTypeDownload)
		{
			post_transmit_info(req_ptr);
		}
	}
}
