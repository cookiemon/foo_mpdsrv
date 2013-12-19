#ifndef SLEEPYTHREAD_H
#define SLEEPYTHREAD_H

#include "common.h"

namespace foo_mpdsrv
{
	/**
	 * Thread that sleeps whenever it has no work
	 * @author Cookiemon
	 */
	class SleepyThread : public CSimpleThread
	{
	private:
		// Event that is pinged when work is received
		win32_event _event;
		// true if the thread should terminate on next occasion
		bool _abort;

	private:
		/**
		 * Not copyable
		 * @author Cookiemon
		 */
		SleepyThread(const SleepyThread&);
	public:
		/**
		 * Creates events
		 * @author Cookiemon
		 */
		SleepyThread();
		/**
		 * Moves thread
		 * @author Cookiemon
		 * @attention Might have race conditions if _abort on at least one thread is true
		 * @param name Name of the playlist to select
		 */
		SleepyThread(SleepyThread&& right);
		/**
		 * Releases the counter and finishes the thread
		 * @author Cookiemon
		 */
		~SleepyThread();
		
		/**
		 * Pings the thread and tells him he got work
		 * @author Cookiemon
		 */
		void Wake();
	protected:
		/**
		 * Threadproc that handles the sleeping and calling of WakeProc
		 * @author Cookiemon
		 * @param p_abort Abort callback
		 * @return 0 (exit code)
		 */
		unsigned int ThreadProc(abort_callback& p_abort);
		/**
		 * Exits the thread
		 * @author Cookiemon
		 * @param p_abort Abort callback
		 */
		void ExitThread();
		/**
		 * Threadproc that is called whenever some work has to be done
		 * @author Cookiemon
		 * @param p_abort Abort callback
		 * @return false iff the thread should terminate
		 */
		virtual bool WakeProc(abort_callback& p_abort) = 0;
	};
}

#endif