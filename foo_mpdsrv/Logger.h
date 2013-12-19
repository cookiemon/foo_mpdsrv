#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>

/**
 * Overloaded stream operator for DWORD
 * @author Cookiemon
 * @param str pfc string
 * @param val DWORD to stream into str
 * @return str
 */
inline pfc::string_base& operator<<(pfc::string_base& str, DWORD val)
{
	str.add_string(pfc::format_uint(val));
	return str;
}
/**
 * Overloaded stream operator for HANDLE
 * @author Cookiemon
 * @param str pfc string
 * @param val HANDLE to stream into str
 * @return str
 */
inline pfc::string_base& operator<<(pfc::string_base& str, HANDLE val)
{
	str.add_string(pfc::format_uint(reinterpret_cast<uintptr_t>(val)));
	return str;
}
/**
 * Overloaded stream operator for std::string
 * @author Cookiemon
 * @param str pfc string
 * @param val std::string to stream into str
 * @return str
 */
inline pfc::string_base& operator<<(pfc::string_base& str, std::string val) { return str << val.c_str(); }

namespace foo_mpdsrv
{
	/**
	 * Returns error string from WINAPI error number
	 * @author Cookiemon
	 * @param errNum number of error
	 * @return Error message that corresponds to errNum
	 */
	inline std::string GetErrString(DWORD errNum)
	{
		LPSTR msg = NULL;
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			errNum,
			0,
			reinterpret_cast<LPSTR>(&msg),
			0,
			NULL);
		std::string strMsg = msg;
		LocalFree(msg);
		return strMsg;
	}

	/**
	 * Returns error number and error string from last WINAPI error
	 * @author Cookiemon
	 * @param lastErrNum Number of last error
	 * @param lastErrStr String of last error
	 */
	inline void GetLastErrString(DWORD& lastErrNum, std::string& lastErrStr)
	{
		lastErrNum = GetLastError();
		lastErrStr = GetErrString(lastErrNum);
	}
	
	/**
	 * Logs to files and foobar console depending on compiler flags
	 * @author Cookiemon
	 */
	class Logger
	{
#ifndef FOO_MPDSRV_DISABLE_LOG
	private:
		static const unsigned int _minLvl = 10;
		unsigned int _logLvl;
#ifdef FOO_MPDSRV_LOGFILE
		std::ofstream _str;
#else
		console::formatter _csl;
#endif
#endif
	public:
		static const unsigned int FINEST = 1;
		static const unsigned int FINER = 5;
		static const unsigned int DBG = 10;
		static const unsigned int WARN = 50;
		static const unsigned int SEVERE = 100;

	private:
		/**
		 * Not copyable
		 * @author Cookiemon
		 * @param Logger unused
		 */
		Logger(const Logger&);
		/**
		 * Not assignable
		 * @author Cookiemon
		 * @param Logger unused
		 */
		Logger& operator=(const Logger&);

	public:
		/**
		 * Initialize logger with a log level
		 * @author Cookiemon
		 * @param logLvl If log level is above _minLvl, messages are logged
		 * otherwise all functions (except for @see Logger::IsLogging ) do nothing
		 */
		explicit Logger(unsigned int logLvl)
#ifndef FOO_MPDSRV_DISABLE_LOG
			: _logLvl(logLvl)
#ifdef FOO_MPDSRV_LOGFILE
			, _str("C:\\logs\\foo_mpd.log", std::fstream::app)
#endif
#endif
		{
			/* Nothing */
		}

		/**
		 * Close log message with a newline
		 */
		~Logger()
		{
#ifdef FOO_MPDSRV_LOGFILE
			_str << "\n";
#endif
		}
		
		/**
		 * Returns if the loggers log level is high enough
		 * to log
		 * @author Cookiemon
		 * @return true iff log message are written to the output
		 */
		bool IsLogging()
		{
#ifndef FOO_MPDSRV_DISABLE_LOG
			return _logLvl >= _minLvl;
#else
			return false;
#endif
		}
		
		/**
		 * Writes a raw string to the log output
		 * @author Cookiemon
		 * @param buf Message to log
		 * @param num Number of characters in buf to log
		 * @return *this
		 */
		Logger& Write(const char* buf, size_t num)
		{
#ifndef FOO_MPDSRV_DISABLE_LOG
			if(IsLogging())
			{
#ifdef FOO_MPDSRV_LOGFILE
				_str.write(buf, num);
#else
				_csl.add_string_nc(buf, num);
#endif
			}
#endif
			return *this;
		}

		/**
		 * Converts and logs a value
		 * @author Cookiemon
		 * @param val Value to log
		 * @return *this
		 */
		template<typename T>
		Logger& Log (const T& val)
		{
#ifndef FOO_MPDSRV_DISABLE_LOG
			if(_logLvl >= _minLvl)
			{
#ifdef FOO_MPDSRV_LOGFILE
				_str << val;
				_str.flush();
#else
				_csl << val;
#endif
			}
#endif
			return *this;
		}
		
		/**
		 * Logs a WINAPI error
		 * @author Cookiemon
		 * @param val String to log with the error
		 * @param errNum Error number of the winapi message that occured
		 * @return *this
		 */
		Logger& LogWinError(const std::string& val, DWORD errNum)
		{
			Log(val);
			Log(": (");
			Log(errNum);
			Log(") ");
			Log(GetErrString(errNum));
			return *this;
		}

		/**
		 * Sets a new log level to this Logger
		 * @author Cookiemon
		 * @param logLvl New log level
		 */
		void SetLogLvl(unsigned int logLvl)
		{
#ifndef FOO_MPDSRV_DISABLE_LOG
			_logLvl = logLvl;
#endif
		}
	};
}

/**
 * Logs to the output
 * @author Cookiemon
 * @param log Logger which is used for output
 * @param val Value that is to be logged
 */
template<typename T>
inline foo_mpdsrv::Logger& operator<<(foo_mpdsrv::Logger& log, T val)
{
	log.Log(val);
	return log;
}

#endif