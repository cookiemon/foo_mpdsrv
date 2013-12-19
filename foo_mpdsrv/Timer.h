#ifndef TIMER_H
#define TIMER_H

#include "common.h"

#ifdef FOO_MPDSRV_TIMER
#pragma comment(lib, "winmm.lib")
#endif

namespace foo_mpdsrv
{
	/**
	 * Timer for performance measurement
	 * Deactivated if not compiled with FOO_MPDSRV_TIMER
	 * @author Cookiemon
	 */
	class Timer
	{
	private:
#ifdef FOO_MPDSRV_TIMER
		DWORD _begin;
#endif
	public:
		/**
		 * Creates a timer initialized to the current time
		 * @author Cookiemon
		 */
		Timer()
#ifdef FOO_MPDSRV_TIMER
			: _begin(timeGetTime())
#endif
		{
		}
		/**
		 * Sets the timer to the current time
		 * @author Cookiemon
		 */
		void StartTimer()
		{
#ifdef FOO_MPDSRV_TIMER
			_begin = timeGetTime();
#endif
		}
		/**
		 * Returns the difference between the stored timer and now
		 * @author Cookiemon
		 * @return Timedifference in msec
		 */
		DWORD GetDifference()
		{
#ifdef FOO_MPDSRV_TIMER
			return timeGetTime() - _begin;
#else
			return 0;
#endif
		}
		/**
		 * Logs the time difference with the given message
		 * Used loglevel is DBG
		 * @author Cookiemon
		 * @param msg Message to write to log
		 */
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