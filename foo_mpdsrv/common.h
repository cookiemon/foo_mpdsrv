#ifndef COMMON_H
#define COMMON_H

#pragma warning(disable: 4505)
#pragma warning(push, 2)
// Microsoft, I hate you. I really really do.
// Breaks std::numeric_limits<>::max()
#define NOMINMAX
#undef WINVER
#define WINVER 0x0502
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0502
#include <ATLHelpers.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <foobar2000.h>
#include <fstream>
#pragma warning(pop)
#include <string>

// For the sake of good!
#undef NULL
#define NULL nullptr

#include "CommandException.h"
#include "Logger.h"

namespace foo_mpdsrv
{
	extern const char* PLUGINNAME;
	extern const wchar_t* PLUGINNAMEW;

	#ifdef FOO_MPDSRV_EXPORTS
	#define FOO_MPDSRV_API __declspec(dllexport)
	#else
	#define FOO_MPDSRV_API __declspec(dllimport)
	#endif

	#ifdef UNICODE
	typedef std::wstring tstring;
	#else
	typedef std::string string;
	#endif

	template<class T, typename CountType = unsigned int>
	class RefCounter
	{
	private:
		static CountType _count;
	private:
		RefCounter(const RefCounter&);
		RefCounter& operator=(const RefCounter&);
	public:
		RefCounter() { _count += 1; }
		~RefCounter() { _count -= 1; }
		CountType getCount() { return _count; }
	};
	template<class T, typename CountType>
	CountType RefCounter<T,CountType>::_count = 0;

	inline bool strstartswithi(const char* left, const char* right)
	{
		unsigned int len = strlen(right);
		if(len > strlen(left))
			return false;
		
		const char* end = left + len;
		while(left != end)
		{
			if(tolower(*left) != tolower(*right))
				return false;
			left += 1;
			right += 1;
		}
		return true;
	}

	struct CompareTolower
	{
		bool operator() (const char left, const char right)
		{
			return tolower(left) == tolower(right);
		}
	};

	inline bool strstartswithi(const std::string& left, const std::string& right)
	{
		if(left.length() < right.length())
			return false;

		return std::equal(right.begin(), right.end(), left.begin(), CompareTolower());
	}

	template<typename T, typename U>
	class Converter
	{
	public:
		void operator()(const T& in, U& out)
		{
			std::stringstream ss;
			ss << in;
			ss >> out;
			if(ss.fail())
				throw CommandException(ACK_ERROR_ARG, "Argument type mismatch");
		}
	};

	template<typename T, typename U>
	inline void ConvertTo(const T& in, U& out)
	{
		Converter<T, U>()(in, out);
	}
}

#endif