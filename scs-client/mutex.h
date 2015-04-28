
#pragma once

#include <pthread.h>

class mutex  
{
public:
	mutex()
	{
		pthread_mutex_init(&_mutex, NULL);
	}
	virtual ~mutex()
	{
		pthread_mutex_destroy(&_mutex);
	}
	void lock()
	{
		pthread_mutex_lock(&_mutex);
	}
	void unlock()
	{
		pthread_mutex_unlock(&_mutex);
	}
	void timed_lock(unsigned long timeout_ms)
	{
		timespec ts;
		ts.tv_sec = timeout_ms / 1000;
		ts.tv_nsec = (timeout_ms % 1000) * 1000000;
		pthread_mutex_timedlock(&_mutex, &ts);
	}

private:
	pthread_mutex_t		_mutex;
};

class scoped_lock
{
public:
	scoped_lock( mutex& mutex_obj ) : _mutex_obj(mutex_obj)
	{
		_mutex_obj.lock();
	}
	~scoped_lock()
	{
		_mutex_obj.unlock();
	}
private:
	mutex& _mutex_obj;
};

class event_object
{
public:
	event_object()
		: _is_set(false)
	{
		pthread_mutex_init(&_mutex, NULL);
		pthread_cond_init(&_cond, NULL);
	}
	~event_object()
	{
		pthread_mutex_destroy(&_mutex);
		pthread_cond_destroy(&_cond);
	}
	int wait(unsigned long timeout_ms = -1)
	{
		int iret = 0;
		pthread_mutex_lock(&_mutex);
		if (!_is_set)
		{
			if (timeout_ms == -1)
			{
				iret = pthread_cond_wait(&_cond, &_mutex);
			}
			else
			{
				struct timeval now;
				struct timespec outtime;
				_gettimeofday(&now, NULL);
				outtime.tv_sec = now.tv_sec + (timeout_ms / 1000);
				outtime.tv_nsec = now.tv_usec * 1000 + (timeout_ms % 1000 * 1000 * 1000);
				iret = pthread_cond_timedwait(&_cond, &_mutex, &outtime);
			}
		}
		_is_set = false;
		pthread_mutex_unlock(&_mutex);
		return iret;
	}
	int set()
	{
		pthread_mutex_lock(&_mutex);
		_is_set = true;
		int iret = pthread_cond_signal(&_cond);
		pthread_mutex_unlock(&_mutex);
		return iret;
	}
private:
	int _gettimeofday( struct timeval *tp, void *tzp )
	{
#ifndef WIN32
		return gettimeofday(tp, tzp);
#else
		time_t clock;
		struct tm tm;
		SYSTEMTIME wtm;

		GetLocalTime(&wtm);
		tm.tm_year     = wtm.wYear - 1900;
		tm.tm_mon     = wtm.wMonth - 1;
		tm.tm_mday     = wtm.wDay;
		tm.tm_hour     = wtm.wHour;
		tm.tm_min     = wtm.wMinute;
		tm.tm_sec     = wtm.wSecond;
		tm. tm_isdst    = -1;
		clock = mktime(&tm);
		tp->tv_sec = (long)clock;
		tp->tv_usec = wtm.wMilliseconds * 1000;

		return (0);
#endif
	}
private:
	pthread_mutex_t		_mutex;
	pthread_cond_t		_cond;
	bool				_is_set;
};