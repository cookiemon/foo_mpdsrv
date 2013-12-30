#include "RWLock.h"
#include "common.h"

namespace foo_mpdsrv
{
	RWLock::RWLock()
	{
		_lock.reset(new SRWLOCK);
		InitializeSRWLock(_lock.get());
	}
	RWLock::~RWLock()
	{
	}
	ReadLockHandle RWLock::LockRead()
	{
		return ReadLockHandle(_lock.get());
	}
	WriteLockHandle RWLock::LockWrite()
	{
		return WriteLockHandle(_lock.get());
	}
	ReadLockHandle::ReadLockHandle(PSRWLOCK lock) : _lock(lock)
	{
		AcquireSRWLockShared(_lock);
	}
	ReadLockHandle::ReadLockHandle(ReadLockHandle&& right)
	{
		_lock = right._lock;
		right._lock = NULL;
	}
	ReadLockHandle::~ReadLockHandle()
	{
		if(_lock != NULL)
			ReleaseSRWLockShared(_lock);
	}
	void ReadLockHandle::Unlock()
	{
		ReleaseSRWLockShared(_lock);
		_lock = NULL;
	}
	WriteLockHandle::WriteLockHandle(PSRWLOCK lock) : _lock(lock)
	{
		AcquireSRWLockExclusive(_lock);
	}
	WriteLockHandle::WriteLockHandle(WriteLockHandle&& right)
	{
		_lock = right._lock;
		right._lock = NULL;
	}
	WriteLockHandle::~WriteLockHandle()
	{
		if(_lock != NULL)
			ReleaseSRWLockExclusive(_lock);
	}
	void WriteLockHandle::Unlock()
	{
		ReleaseSRWLockExclusive(_lock);
		_lock = NULL;
	}
}