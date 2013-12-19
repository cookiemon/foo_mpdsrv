#ifndef CONNECTIONLISTENER_H
#define CONNECTIONLISTENER_H

#include "common.h"
#include "NetworkInformation.h"
#include <WinSock2.h>
#include <vector>

namespace foo_mpdsrv
{
	/**
	 * Listener for new messages
	 * Message are received by @see WindowMessageHandler
	 * because data is received through window message
	 * queue. This only holds the connection information
	 * and opens/closes connections
	 * @author Cookiemon
	 */
	class ConnectionListener
	{
	private:
		NetworkInformation _networkInfo;
		int _lastError;
		std::vector<SOCKET> _socketfds;

	private:
		/**
		 * Not copyable
		 * @author Cookiemon
		 * @param ConnectionListener unused
		 */
		ConnectionListener(const ConnectionListener&);
		/**
		 * Not assignable
		 * @author Cookiemon
		 * @param ConnectionListener unused
		 */
		ConnectionListener& operator=(const ConnectionListener&);

	public:
		
		/**
		 * Constructs a not listening connection
		 * @author Cookiemon
		 */
		ConnectionListener();
		/**
		 * Move constructs the connection
		 * @author Cookiemon
		 * @attention other is invalid after call
		 * @param other Connection to move
		 */
		ConnectionListener(ConnectionListener&& other);
		/**
		 * Closes connection if open
		 * @author Cookiemon
		 */
		~ConnectionListener();

		/**
		 * Starts listening on a specified interface and port
		 * @author Cookiemon
		 * @param addr Address of the interface to listen on
		 * @param port Port to listen on
		 */
		void StartListening(const pfc::stringp& addr, const pfc::stringp& port);
		/**
		 * Closes the connection
		 * @author Cookiemon
		 */
		void StopListening();
		/**
		 * Closes the current connection and restarts listening
		 * on a specified interface and port
		 * @author Cookiemon
		 * @param addr Address of the interface to listen on
		 * @param port Port to listen on
		 */
		void RefreshConnection(const pfc::stringp& addr, const pfc::stringp& port);
		
		/**
		 * Checks if the address is a valid IPv4 or IPv6 address
		 * @author Cookiemon
		 * @param addr Address to check
		 * @return true iff addr is a valid IPv4 or IPv6 address
		 */
		bool IsAddressValid(const pfc::stringp& addr);
		/**
		 * Checks if the port is a valid port. Does not check
		 * if port is currently used by a different application
		 * @author Cookiemon
		 * @param port Port to check
		 * @return true iff port is a valid port to listen on
		 */
		bool IsPortValid(const pfc::stringp& port);
		
		/**
		 * Returns last WSA Error number
		 * @author Cookiemon
		 * @return WSA error
		 */
		int GetLastError() { return _lastError; }

	private:
		/**
		 * Binds to a socket
		 * @author Cookiemon
		 * @param addrinfo Address info structure for binding call
		 * @return Socket number that has been bound to
		 */
		SOCKET BindSocket(ADDRINFOA* addrinfo);
		/**
		 * Unbinds given socket (stops listening)
		 * @author Cookiemon
		 * @param sock Socket to stop listening on
		 */
		void UnbindSocket(SOCKET sock);
	};

}
#endif