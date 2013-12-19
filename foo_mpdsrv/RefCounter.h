#ifndef REFCOUNTER_H
#define REFCOUNTER_H

namespace foo_mpdsrv
{
	/**
	 * Reference counter.
	 * Only counts how many RefCounter<T, CountType> of a
	 * given T and CountType exists. Can be used e.g. to
	 * count how many instances of a class exist
	 * @author Cookiemon
	 */
	template<class T, typename CountType = unsigned int>
	class RefCounter
	{
	private:
		// Counter
		static CountType _count;
	private:
		/**
		 * Uncopyable
		 * @author Cookiemon
		 * @param RefCounter unused
		 */
		RefCounter(const RefCounter&);
		/**
		 * Unassignable
		 * @author Cookiemon
		 * @param RefCounter unused
		 */
		RefCounter& operator=(const RefCounter&);
	public:
		/**
		 * Increase counter by 1
		 * @author Cookiemon
		 */
		RefCounter() { _count += 1; }
		/**
		 * Decrease counter by 1
		 * @author Cookiemon
		 */
		~RefCounter() { _count -= 1; }
		/**
		 * Returns current counter
		 * @author Cookiemon
		 * @return Current counter
		 */
		CountType GetCount() { return _count; }
	};

	template<class T, typename CountType>
	CountType RefCounter<T, CountType>::_count = 0;
}

#endif