#include "SleepyThread.h"

namespace foo_mpdsrv
{
	SleepyThread::SleepyThread()
	{
		_abort = false;
		_event.create(false, false);
	}
	SleepyThread::SleepyThread(SleepyThread&& right)
	{
		_abort = right._abort;
		_event.set(right._event.get());
		right._event.set(NULL);
	}
	SleepyThread::~SleepyThread()
	{
		_event.set_state(true);
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
				Logger log(Logger::FINEST);
				log.Log("Thread received some work.");
				run = WakeProc(p_abort);
			}
		}
		p_abort.check();
		Logger log(Logger::DBG);
		log.Log("Event ");
		log.Log(_event.get());
		log.Log(" canceled.");
		return 0;
	}
}