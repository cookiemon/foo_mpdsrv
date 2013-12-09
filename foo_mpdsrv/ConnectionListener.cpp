#include "common.h"
#include "ConnectionListener.h"
#include "WindowMessageHandler.h"
#include <cstdio>
#include <iostream>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>

namespace foo_mpdsrv
{

	ConnectionListener::ConnectionListener() : _lastError(0)
	{
	}

	ConnectionListener::~ConnectionListener()
	{
		StopListening();
	}

	void ConnectionListener::StartListening(const pfc::stringp& addr, const pfc::stringp& port)
	{
		TRACK_CALL_TEXT("ConnectionListener::StartListening()");
		_networkInfo.RequestAddressInfo(addr, port);
		for(ADDRINFOA* i = _networkInfo.GetAddressInfo(); i != NULL; i = i->ai_next)
		{
			if(i->ai_family == PF_INET || i->ai_family == PF_INET6)
			{
				Logger(Logger::DBG) << "Binding interface: " << addr;
				SOCKET fd = BindSocket(i);
				if(fd != SOCKET_ERROR)
					_socketfds.push_back(fd);
				else
					Logger(Logger::SEVERE).LogWinError("Binding failed", _lastError);
			}
		}
		if(_socketfds.empty())
		{
			Logger(Logger::SEVERE).LogWinError("Could not bind to socket", _lastError);
			return;
		}
		else
		{
			Logger(Logger::DBG) << "Bound to " << _socketfds.size() << " addresses";
		}
	}

	void ConnectionListener::StopListening()
	{
		TRACK_CALL_TEXT("ConnectionListener::StopListening()");
		for(size_t i = _socketfds.size() - 1; !_socketfds.empty(); --i)
		{
			UnbindSocket(_socketfds[i]);
			_socketfds.pop_back();
		}
	}

	void ConnectionListener::RefreshConnection(const pfc::stringp& addr, const pfc::stringp& port)
	{
		TRACK_CALL_TEXT("ConnectionListener::RefreshConnection()");
		StopListening();
		StartListening(addr, port);
	}

	SOCKET ConnectionListener::BindSocket(ADDRINFOA* addressinfo)
	{
		TRACK_CALL_TEXT("ConnectionListener::BindSocket()");
		SOCKET socketfd = socket(addressinfo->ai_family, addressinfo->ai_socktype, addressinfo->ai_protocol);
		if(socketfd == SOCKET_ERROR)
		{
			_lastError = WSAGetLastError();
			Logger(Logger::SEVERE).LogWinError("Socket creation failed", _lastError);
			return static_cast<SOCKET>(SOCKET_ERROR);
		}

		_lastError = bind(socketfd, addressinfo->ai_addr, addressinfo->ai_addrlen);

		if(_lastError == ERROR_SUCCESS)
		{
			_lastError = WSAAsyncSelect(socketfd, core_api::get_main_window(),
				WindowMessageHandler::HandledMessage,
				(FD_ACCEPT | FD_CLOSE | FD_READ));
			if(_lastError == ERROR_SUCCESS)
			{
				_lastError = listen(socketfd, 3);
				if(_lastError == ERROR_SUCCESS)
				{
					return socketfd;
				}
				else
				{
					_lastError = WSAGetLastError();
					Logger(Logger::SEVERE).LogWinError("Socket listen failed", _lastError);
				}
			}
			else
			{
				_lastError = WSAGetLastError();
				Logger(Logger::SEVERE).LogWinError("Socket Asyncselect failed", _lastError);
			}
		}
		else
		{
			_lastError = WSAGetLastError();
			Logger(Logger::SEVERE).LogWinError("Socket bind failed", _lastError);
		}
		UnbindSocket(socketfd);
		return static_cast<SOCKET>(SOCKET_ERROR);
	}

	void ConnectionListener::UnbindSocket(SOCKET sock)
	{
		TRACK_CALL_TEXT("ConnectionListener::UnbindSocket()");
		if(sock != static_cast<SOCKET>(SOCKET_ERROR))
			closesocket(sock);
	}
}