#ifndef _MULTI_HTTP_
#define _MULTI_HTTP_


#include <list>
#include "curl/multi.h"
#include "thread.h"
#include "http_request.h"
#include "mutex.h"

#ifdef RECORD_LOG
#include "Log4cplusSwitch.h"
#endif


#ifndef LOG_CLASS_DECLARE
#	define LOG_CLASS_DECLARE()
#endif

#ifndef LOG_CLASS_IMPLEMENT
#	define LOG_CLASS_IMPLEMENT(a, b)
#endif

#ifndef LOG_ERROR
#	define LOG_ERROR(x)
#endif

#ifndef LOG_DEBUG
#	define LOG_DEBUG(x)
#endif

#ifndef LOG4CPLUS_DEBUG
#	define LOG4CPLUS_DEBUG(x)
#endif

class multi_http_msg_sender
{
public:
	virtual void post_message(unsigned long msg_id, unsigned long param1 = 0, unsigned long param2 = 0) {}
};

class multi_http
	: public thread
{
	typedef std::list<http_request*>	RequestList;
public:
	multi_http();
	virtual ~multi_http();

	void add_request(http_handler_base* handler_ptr);
	void del_request(http_handler_base* handler_ptr);

protected:
	virtual void run();
	virtual void cleanup();

public:
	void post_complete_message(http_request* req_ptr, unsigned long result);
	void post_response_header(http_request* req_ptr);
	void post_transmit_info(http_request* req_ptr);

	// running in curl thread
private:
	void remove_deleted_request();
	void run_waiting_request();
	void build_request(http_request* req_ptr);
	void build_http_parameters(CURL* eh, http_request* req_ptr);

	int multi_perform();
	void handle_completed(CURL* curl, int result);
	void clear_all_requests();

	// running in call thread
private:
	http_request* create_request(http_handler_base* handler_ptr);
	void do_delete_request(http_handler_base* handler_ptr);
	void modify_upload_file_size(http_request* request, http_handler_base* handler_ptr);
	void collect_transmit_info();

	// message dispatch
public:
	static void handle_message(unsigned long msg_id, unsigned long param1, unsigned long param2);
	void post_message(unsigned long msg_id, unsigned long param1 = 0, unsigned long param2 = 0);
	void set_message_sender(multi_http_msg_sender* sender) { _msg_sender = sender; }

	static const std::string curl_error_string(int err_code);

private:
	CURLM*							_curlm;
	CURLMsg*						_msg;
	mutex							_request_list_mutex;
	event_object					_add_event;

	RequestList						_wait_add_requests;
	RequestList						_wait_del_requests;
	RequestList						_running_requests;

	multi_http_msg_sender*			_msg_sender;

public:
	LOG_CLASS_DECLARE();
};

#endif