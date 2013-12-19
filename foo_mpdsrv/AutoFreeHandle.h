#ifndef AUTOFREEHANDLE_H
#define AUTOFREEHANDLE_H

#include "common.h"

namespace foo_mpdsrv
{
	/**
	 * Locks and unlocks a metadb handle
	 * for database manipulation
	 * @author Cookiemon
	 * @todo refactor
	 */
	class FooMetadbHandleLocker
	{
	public:
		/**
		 * Locks handle
		 * @author Cookiemon
		 * @param hnd Handle to lock
		 */
		void Lock(metadb_handle& hnd)
		{
			hnd.metadb_lock();
		}
		/**
		 * Unlocks handle
		 * @author Cookiemon
		 * @param hnd Handle to unlock
		 */
		void Unlock(metadb_handle& hnd)
		{
			hnd.metadb_unlock();
		}
	};
	/**
	 * Locks and unlocks a metadb handle pointer
	 * for database manipulation
	 * @author Cookiemon
	 * @todo refactor
	 */
	class FooMetadbHandlePtrLocker
	{
	public:
		/**
		 * Locks handle
		 * @author Cookiemon
		 * @param hnd Pointer to handle pointer to lock
		 */
		void Lock(metadb_handle_ptr& hnd)
		{
			hnd->metadb_lock();
		}
		/**
		 * Unlocks handle
		 * @author Cookiemon
		 * @param hnd Pointer to handle to unlock
		 */
		void Unlock(metadb_handle_ptr& hnd)
		{
			hnd->metadb_unlock();
		}
	};
	/**
	 * Automatically locks and unlocks handles
	 * @author Cookiemon
	 * @todo refactor
	 */
	template<typename T, typename U>
	class AutoFreeHandle
	{
	private:
		T* t;
		AutoFreeHandle(const AutoFreeHandle<T, U>&);
	public:
		/**
		 * Locks handle
		 * @author Cookiemon
		 * @param handleOn Handle that should be held
		 */
		explicit AutoFreeHandle(T& handleOn) : t(&handleOn)
		{
			U().Lock(handleOn);
		}
		/**
		 * Frees handle
		 * @author Cookiemon
		 */
		~AutoFreeHandle()
		{
			U().Unlock(*t);
		}
	};

	/**
	 * Typedef for an automatically unlocking metadb handle
	 */
	typedef AutoFreeHandle<metadb_handle, FooMetadbHandleLocker> FooMetadbHandleLock;
	/**
	 * Typedef for an automatically unlocking metadb handle ptr
	 */
	typedef AutoFreeHandle<metadb_handle_ptr, FooMetadbHandlePtrLocker> FooMetadbHandlePtrLock;
}

#endif