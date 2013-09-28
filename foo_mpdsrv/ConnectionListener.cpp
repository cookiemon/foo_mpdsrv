#include "common.h"
#include "ConnectionListener.h"
#include "WindowMessageHandler.h"
#include <cstdio>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

//TODO: IMPROVE
inline void popupNetworkError(const char* message, int errorNum = -1)
{
	if(errorNum == -1)
		errorNum = WSAGetLastError();
	//console::formatter form;
	//form << message << " (" << errorNum << ", " << gai_strerrorA(errorNum) << ")";
	//popup_message::g_show(form, PLUGINNAME, popup_message::icon_error);
	std::ofstream err("C:\\logs\\foo_mpd.err", std::fstream::app);
	err << message << " (" << errorNum << ", " << gai_strerrorA(errorNum) << ")" << std::endl;
}

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
		_networkInfo.RequestAddressInfo(addr, port);
		for(ADDRINFOA* i = _networkInfo.GetAddressInfo(); i != NULL; i = i->ai_next)
		{
			if(i->ai_family == PF_INET || i->ai_family == PF_INET6)
			{
				SOCKET fd = BindSocket(i);
				if(fd != SOCKET_ERROR)
					_socketfds.push_back(fd);
			}
		}
		if(_socketfds.empty())
		{
			popupNetworkError("Could not bind to any socket", _lastError);
			return;
		}
		else
		{
			popupNetworkError("Bound to some addresses", _socketfds.size());
		}
	}

	void ConnectionListener::StopListening()
	{
		for(size_t i = _socketfds.size() - 1; !_socketfds.empty(); --i)
		{
			UnbindSocket(_socketfds[i]);
			_socketfds.pop_back();
		}
	}

	void ConnectionListener::RefreshConnection(const pfc::stringp& addr, const pfc::stringp& port)
	{
		StopListening();
		StartListening(addr, port);
	}

	SOCKET ConnectionListener::BindSocket(ADDRINFOA* addressinfo)
	{
		SOCKET socketfd = socket(addressinfo->ai_family, addressinfo->ai_socktype, addressinfo->ai_protocol);
		if(socketfd == SOCKET_ERROR)
		{
			_lastError = WSAGetLastError();
			return static_cast<SOCKET>(SOCKET_ERROR);
		}

		_lastError = bind(socketfd, addressinfo->ai_addr, addressinfo->ai_addrlen);

		if(_lastError == ERROR_SUCCESS)
		{
			_lastError = WSAAsyncSelect(socketfd, core_api::get_main_window(),
				WindowMessageHandler::HandledMessage,
				(FD_ACCEPT | FD_CLOSE | FD_READ)
				);
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
					popupNetworkError("Listen on socket failed", _lastError);
				}
			}
			else
			{
				_lastError = WSAGetLastError();
				popupNetworkError("Could not request receive notifications", _lastError);
			}
		}

		_lastError = WSAGetLastError();
		UnbindSocket(socketfd);
		return static_cast<SOCKET>(SOCKET_ERROR);
	}

	void ConnectionListener::UnbindSocket(SOCKET sock)
	{
		if(sock != static_cast<SOCKET>(SOCKET_ERROR))
			closesocket(sock);
	}
}