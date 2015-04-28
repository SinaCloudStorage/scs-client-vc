#pragma once


template < class _Type >
class singleton_ex
{
public:
    static void create_instance()
    {
		_instance = new _Type();
    }
    static _Type* get_instance()
    {
        return _instance;
    }
    static void destroy_instance()
    {
		if (_instance)
		{
			delete _instance;
			_instance = NULL;
		}
    }

protected:
	static _Type*  _instance;
};

template<class _Type>
_Type*  singleton_ex<_Type>::_instance;