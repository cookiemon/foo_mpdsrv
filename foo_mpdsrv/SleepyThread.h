#ifndef SLEEPYTHREAD_H
#define SLEEPYTHREAD_H

#include "common.h"

namespace foo_mpdsrv
{
	class SleepyThread : public CSimpleThread
	{
	private:
		win32_event _event;
		bool _abort;

	private:
		SleepyThread(const SleepyThread&);
	public:
		SleepyThread();
		SleepyThread(SleepyThread&& right);
		~SleepyThread();

		void Wake();
	protected:
		unsigned int ThreadProc(abort_callback& p_abort);
		void ExitThread();
		virtual bool WakeProc(abort_callback& p_abort) = 0;
	};
}

#endif