#ifndef COMMANDEXCEPTION_H
#define COMMANDEXCEPTION_H

#include <exception>
#include <string>
#include <sstream>

namespace foo_mpdsrv
{
	enum ACK
	{
		ACK_ERROR_NOT_LIST       =  1,
		ACK_ERROR_ARG            =  2,
		ACK_ERROR_PASSWORD       =  3,
		ACK_ERROR_PERMISSION     =  4,
		ACK_ERROR_UNKNOWN        =  5,
		ACK_ERROR_NO_EXIST       = 50,
		ACK_ERROR_PLAYLIST_MAX   = 51,
		ACK_ERROR_SYSTEM         = 52,
		ACK_ERROR_PLAYLIST_LOAD  = 53,
		ACK_ERROR_UPDATE_ALREADY = 54,
		ACK_ERROR_PLAYER_SYNC    = 55,
		ACK_ERROR_EXIST          = 56
	};
	class CommandException : public std::exception
	{
	private:
		ACK _error;
	public:
		CommandException(ACK error, const char* msg) : std::exception(msg), _error(error)
		{
		}

		int GetError() const
		{
			return _error;
		}
	};
}

#endif