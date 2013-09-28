#include "SleepyThread.h"

namespace foo_mpdsrv
{
	SleepyThread::SleepyThread()
	{
		_event.create(false, false);
	}
	SleepyThread::SleepyThread(SleepyThread&& right)
	{
		_event.set(right._event.get());
		right._event.set(NULL);
	}
	SleepyThread::~SleepyThread()
	{
	}

	void SleepyThread::Wake()
	{
		_event.set_state(true);
	}

	unsigned int SleepyThread::ThreadProc(abort_callback& p_abort)
	{
		while(true)
		{
			_event.wait_for(-1);
			p_abort.check();
			if(!WakeProc(p_abort))
				return 0;
		}
	}
}