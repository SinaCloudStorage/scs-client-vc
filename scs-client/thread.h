#ifndef _THREAD_H_
#define _THREAD_H_


#include <pthread.h>

class thread
{
public:
	thread(void) : _is_created(false) {}
	virtual ~thread(void) {}

	static void* cleanup_func(void* arg)
	{
		thread* thread_ptr = (thread*)arg;
		thread_ptr->cleanup();
		return NULL;
	}

	static void* thread_func(void* arg)
	{
		int state, oldstate;
		state = PTHREAD_CANCEL_ENABLE;
		pthread_setcancelstate(state, &oldstate);

		int type, oldtype;
		type = PTHREAD_CANCEL_DEFERRED;
		pthread_setcanceltype(type, &oldtype);

		thread* thread_ptr = (thread*)arg;
		pthread_cleanup_push(thread::cleanup_func, arg);
		thread_ptr->run();
		pthread_cleanup_pop(0);
		return NULL;
	}

	bool start()
	{
		_is_created = (0 == pthread_create(&_thread, NULL, thread::thread_func, this));
		return _is_created;
	}

	bool stop()
	{
		if (is_running())
		{
			int iret = pthread_cancel(_thread);
			if (iret != 0)
				return false;

			void* status = NULL;
			pthread_join(_thread, &status);
			if (status != PTHREAD_CANCELED)
				return false;
		}
		return true;
	}

	bool is_in_thread()
	{
		return _is_created && pthread_equal(_thread, pthread_self()) != 0;
	}

	bool is_running()
	{
		if (!_is_created)
			return false;

		int kill_rc = pthread_kill(_thread,0);
		return (kill_rc != ESRCH && kill_rc != EINVAL);
	}

	void test_stop()
	{
		pthread_testcancel();
	}

protected:
	virtual void run() = 0;
	virtual void cleanup() = 0;

private:
	pthread_t					_thread;
	bool						_is_created;
};


#endif