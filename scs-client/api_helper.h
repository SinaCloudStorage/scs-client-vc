#ifndef _API_HELPER_H_
#define _API_HELPER_H_

#include "json/json.h"
#include "base/string_number_conversions.h"


class api_helper
{
public:
	static std::string asString( Json::Value& js_value )
	{
		if (js_value.empty())
		{
			return "";
		}
		else if (js_value.isString())
		{
			return js_value.asString();
		}
		else if (js_value.isIntegral())
		{
			if (js_value.isUInt())
			{
				uint64 uint_value = js_value.asUInt();
				return Int64ToString(uint_value);
			}
			else if (js_value.isInt())
			{
				int64 int_value = js_value.asInt();
				return Int64ToString(int_value);
			}
			else if (js_value.isBool())
			{
				bool bool_value = js_value.asBool();
				return IntToString(bool_value);
			}
			else
			{
				throw std::exception("api json error!");
			}
		}
		else
		{
			throw std::exception("api json error!");
		}
	}

	static uint32 asUInt( Json::Value& js_value )
	{
		if (js_value.isIntegral())
		{
			if (js_value.isUInt())
			{
				return js_value.asUInt();
			}
			else if (js_value.isInt())
			{
				return (uint32)js_value.asInt();
			}
			else if (js_value.isBool())
			{
				return (uint32)js_value.asBool();
			}
			else
			{
				throw std::exception("api json error!");
			}
		}
		else if (js_value.isString())
		{
			std::string str_value = js_value.asString();
			uint32 ret;
			if (StringToInt(str_value, (int*)&ret))
			{
				return ret;
			}
			else
			{
				throw std::exception("api json error!");
			}
		}
		else
		{
			throw std::exception("api json error!");
		}
	}

	static bool asBool(Json::Value& js_value)
	{
		if (js_value.isIntegral())
		{
			if (js_value.isBool())
			{
				return js_value.asBool();
			}
		}
		return false;
	}

	static uint64 asUInt64( Json::Value& js_value )
	{
		if (js_value.isIntegral())
		{
			if (js_value.isUInt())
			{
				return js_value.asUInt();
			}
			else if (js_value.isInt())
			{
				return (uint64)js_value.asInt();
			}
			else if (js_value.isBool())
			{
				return (uint64)js_value.asBool();
			}
			else
			{
				throw std::exception("api json error!");
			}
		}
		else if (js_value.isString())
		{
			std::string str_value = js_value.asString();
			uint64 ret;
			if (StringToInt64(str_value, (int64*)&ret))
			{
				return ret;
			}
			else
			{
				throw std::exception("api json error!");
			}
		}
		else
		{
			throw std::exception("api json error!");
		}
	}

	static bool isExist( Json::Value& js_value )
	{
		return js_value != Json::Value::null;
	}
};

#endif