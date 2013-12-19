#ifndef CONVERTER_H
#define CONVERTER_H

namespace foo_mpdsrv
{
	/**
	 * Converts type T to type U
	 * @author Cookiemon
	 * @tparam T A type
	 * @tparam T A type
	 */
	template<typename T, typename U>
	class Converter
	{
	public:
		/**
		 * Converts types via a stringstream
		 * @author Cookiemon
		 * @param in Value to convert
		 * @param out Output of converted value
		 * @throw CommandException When conversion from T to U failed (errorcode: ACK_ERROR_ARG)
		 */
		void operator()(const T& in, U& out)
		{
			std::stringstream ss;
			ss << in;
			ss >> out;
			if(ss.fail())
				throw CommandException(ACK_ERROR_ARG, "Argument type mismatch");
		}
	};

	/**
	 * Converts value from type T to U
	 * @author Cookiemon
	 * @tparam T A type
	 * @tparam U A type
	 * @param in Value to convert
	 * @param out Output of converted value
	 */
	template<typename T, typename U>
	inline void ConvertTo(const T& in, U& out)
	{
		Converter<T, U>()(in, out);
	}
}

#endif