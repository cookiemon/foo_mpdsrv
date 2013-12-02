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
		_event.set_state(true);
		AbortThread();
	}

	void SleepyThread::Wake()
	{
		_event.set_state(true);
	}

	unsigned int SleepyThread::ThreadProc(abort_callback& p_abort)
	{
		while(true)
		{
			if(_event.wait_for(INFINITE))
			{
				p_abort.check();
				Logger log(Logger::FINEST);
				log.Log("=> Thread do work now\n");
				bool foo = WakeProc(p_abort);
				if(!foo)
				//if(!WakeProc(p_abort))
					return 0;
			}
			else
			{
				Logger log(Logger::FINEST);
				HANDLE evt = _event.get();
				log.Log("=> Event ");
				log.Log(evt);
				log.Log(" Timeout\n");
			}
		}
	}
}