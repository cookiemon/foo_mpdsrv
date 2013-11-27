#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>

namespace foo_mpdsrv
{
	class Logger
	{
#ifndef FOO_MPDSRV_DISABLE_LOG
	private:
		static const unsigned int _minLvl = 10;
		unsigned int _logLvl;
		std::ofstream _str;
#endif
	public:
		static const unsigned int FINEST = 1;
		static const unsigned int FINER = 5;
		static const unsigned int DBG = 10;
		static const unsigned int WARN = 50;
		static const unsigned int SEVERE = 100;

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
				_str.write(buf, num);
			}
#endif
			return *this;
		}

		explicit Logger(unsigned int logLvl)
#ifndef FOO_MPDSRV_DISABLE_LOG
			: _logLvl(logLvl), _str("C:\\logs\\foo_mpd.log", std::fstream::app)
#endif
		{
			/* Nothing */
		}

		Logger(Logger&& other)
#ifndef FOO_MPDSRV_DISABLE_LOG
			: _logLvl(other._logLvl), _str(std::move(other._str))
#endif
		{
			/* nothing */
		}

		template<typename T>
		Logger& Log (const T& val)
		{
#ifndef FOO_MPDSRV_DISABLE_LOG
			if(_logLvl >= _minLvl)
				_str << val;
			_str.flush();
#endif
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