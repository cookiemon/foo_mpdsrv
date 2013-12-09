#ifndef TIMER_H
#define TIMER_H

#include "common.h"

#ifdef FOO_MPDSRV_TIMER
#pragma comment(lib, "winmm.lib")
#endif

namespace foo_mpdsrv
{
	class Timer
	{
	private:
#ifdef FOO_MPDSRV_TIMER
		DWORD _begin;
#endif
	public:
		Timer()
#ifdef FOO_MPDSRV_TIMER
			: _begin(timeGetTime())
#endif
		{
		}
		void StartTimer()
		{
#ifdef FOO_MPDSRV_TIMER
			_begin = timeGetTime();
#endif
		}
		DWORD GetDifference()
		{
#ifdef FOO_MPDSRV_TIMER
			return timeGetTime() - _begin;
#else
			return 0;
#endif
		}
		void LogDifference(const std::string& msg)
		{
#ifdef FOO_MPDSRV_TIMER
			DWORD time = GetDifference();
			Logger(Logger::DBG) << msg << "( " << time << "ms)";
#endif
		}
	};
}

#endif