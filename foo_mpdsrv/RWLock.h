#ifndef RWLOCK_H
#define RWLOCK_H

#include "common.h"
#include <memory>

namespace foo_mpdsrv
{
	class ReadLockHandle
	{
	private:
		PSRWLOCK _lock;
		ReadLockHandle(ReadLockHandle&);
	public:
		ReadLockHandle(PSRWLOCK lock);
		ReadLockHandle(ReadLockHandle&& right);
		~ReadLockHandle();
		void Unlock();
	};

	class WriteLockHandle
	{
	private:
		PSRWLOCK _lock;
		WriteLockHandle(WriteLockHandle&);
	public:
		WriteLockHandle(PSRWLOCK lock);
		WriteLockHandle(WriteLockHandle&& right);
		~WriteLockHandle();
		void Unlock();
	};

	class RWLock
	{
	private:
		std::unique_ptr<SRWLOCK> _lock;
	public:
		RWLock();
		~RWLock();

		ReadLockHandle LockRead();
		WriteLockHandle LockWrite();
	};
}

#endif