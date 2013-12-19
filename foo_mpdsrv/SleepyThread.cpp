#include "SleepyThread.h"
#include <assert.h>

namespace foo_mpdsrv
{
	SleepyThread::SleepyThread()
	{
		_abort = false;
		_event.create(false, false);
	}
	SleepyThread::SleepyThread(SleepyThread&& right)
	{
		assert(!right.IsThreadActive());
		assert(!IsThreadActive());
		_abort = right._abort;
		_event.set(right._event.get());
		right._event.set(NULL);
		right._abort = true;
	}
	SleepyThread::~SleepyThread()
	{
		ExitThread();
	}

	void SleepyThread::ExitThread()
	{
		_abort = true;
		Wake();
	}

	void SleepyThread::Wake()
	{
		_event.set_state(true);
	}

	unsigned int SleepyThread::ThreadProc(abort_callback& p_abort)
	{
		bool run = true;
		while(run)
		{
			run = _event.wait_for(INFINITE);
			p_abort.check();

			run = run && !_abort;
			if(run)
			{
				Logger(Logger::FINEST) << "Thread received some work.";
				run = WakeProc(p_abort);
			}
		}
		p_abort.check();
		Logger(Logger::DBG) << "Event " << _event.get() << " canceled.";
		return 0;
	}
}