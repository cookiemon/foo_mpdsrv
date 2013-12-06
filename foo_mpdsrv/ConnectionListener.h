#ifndef CONNECTIONLISTENER_H
#define CONNECTIONLISTENER_H

#include "common.h"
#include "NetworkInformation.h"
#include <WinSock2.h>
#include <vector>

namespace foo_mpdsrv
{
	class ConnectionListener
	{
	private:
		NetworkInformation _networkInfo;
		int _lastError;
		std::vector<SOCKET> _socketfds;

	private:
		ConnectionListener(const ConnectionListener&);
		ConnectionListener& operator=(const ConnectionListener&);

	public:

		ConnectionListener();
		ConnectionListener(ConnectionListener&& other);
		~ConnectionListener();

		void StartListening(const pfc::stringp& addr, const pfc::stringp& port);
		void StopListening();

		void RefreshConnection(const pfc::stringp& addr, const pfc::stringp& port);

		bool IsAddressValid(const pfc::stringp& addr);
		bool IsPortValid(const pfc::stringp& port);

		int GetLastError() { return _lastError; }

	private:
		SOCKET BindSocket(ADDRINFOA* addrinfo);
		void UnbindSocket(SOCKET sock);
	};

}
#endif