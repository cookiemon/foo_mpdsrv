#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include "common.h"
#include <string>

namespace foo_mpdsrv
{
	/**
	 * Typedef for unicode/non-unicode strings
	 * equivalent of TCHAR for stl strings
	 * @author Cookiemon
	 */
	typedef std::basic_string<TCHAR> tstring;
	
	/**
	 * Does a case insensitive comparison of two characters
	 * @author Cookiemon
	 */
	class CompareTolower
	{
	public:
		/**
		 * Compares the lowercase version of both characters
		 * @author Cookiemon
		 * @param left A character
		 * @param right A character
		 * @return true iff lowercase version of both characters are equal,
		 */
		bool operator() (const char left, const char right)
		{
			return tolower(left) == tolower(right);
		}
	};

	/**
	 * Checks if a string starts with a specified sequence
	 * Does case insensitive comparison
	 * @author Cookiemon
	 * @param left String whose start is checked
	 * @param right Start sequence that is checked for
	 * @return true iff left starts with right as a sequence
	 */
	inline bool StrStartsWithLC(const std::string& left, const std::string& right)
	{
		if(left.length() < right.length())
			return false;

		return std::equal(right.begin(), right.end(), left.begin(), CompareTolower());
	}

	/**
	 * Removes whitespaces from the beginning of a string
	 * @author Cookiemon
	 * @param str String from which the whitespaces are removed
	 */
	inline void TrimLeft(std::string& str)
	{
		str.erase(str.begin(), std::find_if_not(str.begin(), str.end(), &isspace));
	}

	/**
	 * Removes whitespaces from the end of a string
	 * @author Cookiemon
	 * @param str String from which the whitespaces are removed
	 */
	inline void TrimRight(std::string& str)
	{
		str.erase(std::find_if_not(str.rbegin(), str.rend(), &isspace).base(), str.end());
	}

	/**
	 * Removes whitespaces from the beginning and end of a string
	 * @author Cookiemon
	 * @param str String from which the whitespaces are removed
	 */
	inline void Trim(std::string& str)
	{
		TrimRight(str);
		TrimLeft(str);
	}
}

#endif