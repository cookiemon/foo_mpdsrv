#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>

namespace foo_mpdsrv
{

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

	inline void GetLastErrString(DWORD& lastErrNum, std::string& lastErrStr)
	{
		lastErrNum = GetLastError();
	}

	class Logger
	{
#ifndef FOO_MPDSRV_DISABLE_LOG
	private:
		static const unsigned int _minLvl = 0;
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

		~Logger()
		{
#ifdef FOO_MPDSRV_LOGFILE
			_str << "\n";
#endif
		}

		bool IsLogging()
		{
#ifndef FOO_MPDSRV_DISABLE_LOG
			return _logLvl >= _minLvl;
#else
			return false;
#endif
		}

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

		Logger(Logger&& other)
#ifndef FOO_MPDSRV_DISABLE_LOG
			: _logLvl(other._logLvl)
#ifdef FOO_MPDSRV_LOGFILE
			, _str(std::move(other._str))
#endif
#endif
		{
			/* nothing */
		}

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
				std::stringstream sstr;
				sstr << val;
				_csl << sstr.str().c_str();
#endif
			}
#endif
			return *this;
		}

		template<>
		Logger& Log<std::string> (const std::string& val)
		{
#ifndef FOO_MPDSRV_DISABLE_LOG
			if(_logLvl >= _minLvl)
			{
#ifdef FOO_MPDSRV_LOGFILE
				_str << val;
				_str.flush();
#else
				_csl << val.c_str();
#endif
			}
#endif
			return *this;
		}
		
		Logger& LogWinError(const std::string& val, DWORD errNum)
		{
			Log(val);
			Log(": (");
			Log(errNum);
			Log(") ");
			Log(GetErrString);
			return *this;
		}

	private:
		Logger(const Logger&);
	public:
		void SetLogLvl(unsigned int logLvl)
		{
#ifndef FOO_MPDSRV_DISABLE_LOG
			_logLvl = logLvl;
#endif
		}
	};
}

#endif