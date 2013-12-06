#ifndef AUTOFREEHANDLE_H
#define AUTOFREEHANDLE_H

#include "common.h"

namespace foo_mpdsrv
{
	class FooMetadbHandleLocker
	{
	public:
		void lock(metadb_handle& hnd)
		{
			hnd.metadb_lock();
		}
		void unlock(metadb_handle& hnd)
		{
			hnd.metadb_unlock();
		}
	};
	class FooMetadbHandlePtrLocker
	{
	public:
		void lock(metadb_handle_ptr& hnd)
		{
			hnd->metadb_lock();
		}
		void unlock(metadb_handle_ptr& hnd)
		{
			hnd->metadb_unlock();
		}
	};
	template<typename T, typename U>
	class AutoFreeHandle
	{
	private:
		T* t;
		AutoFreeHandle(const AutoFreeHandle<T, U>&);
	public:
		explicit AutoFreeHandle(T& handleOn) : t(&handleOn)
		{
			U().lock(handleOn);
		}
		~AutoFreeHandle()
		{
			U().unlock(*t);
		}
	};

	typedef AutoFreeHandle<metadb_handle, FooMetadbHandleLocker> FooMetadbHandleLock;
	typedef AutoFreeHandle<metadb_handle_ptr, FooMetadbHandlePtrLocker> FooMetadbHandlePtrLock;
}

#endif