#ifndef PFCEXTENSIONS_H
#define PFCEXTENSIONS_H

#include <functional>

namespace std
{
	/**
	 * Class calculating hash for pfc string
	 * uses method for std::string
	 * @author Cookiemon
	 */
	template<>
	class hash<pfc::string8>
	{
	public:
		/**
		 * Calculates pseudorandom number from a string
		 * @author Cookiemon
		 * @param val Value to calculate the number for
		 * @return "Random" positive integer
		 */
		size_t operator()(const pfc::string8& val) const
		{
			return std::hash<std::string>()(val.get_ptr());
		}
	};
}

#endif