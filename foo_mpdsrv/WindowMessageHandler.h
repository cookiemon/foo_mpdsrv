#ifndef WINDOWMESSAGEHANDLER_H
#define WINDOWMESSAGEHANDLER_H

#include "common.h"
#include "MPDMessageHandler.h"
#include "ThreadedEraseMap.h"
#include <map>

namespace foo_mpdsrv
{
	/**
	 * Handles window messages for receiving data
	 * @author Cookiemon
	 */
	class WindowMessageHandler : public message_filter_impl_base
	{
	public:
		static const unsigned int HandledMessage = WM_APP + 'C' + 'o' + 'o' + 'k' + 'i' + 'e' + 's';

	private:
#ifdef FOO_MPDSRV_THREADED
		// Threaded erase because deletion of a message handler might need main thread to finish
		typedef ThreadedEraseMap<SOCKET, MPDMessageHandler> MessageHandlerStorage;
#else
		typedef std::map<SOCKET, MPDMessageHandler> MessageHandlerStorage;
#endif
		typedef std::pair<SOCKET, MPDMessageHandler> MessageHandlerStoragePair;
		char _buffer[256];
		MessageHandlerStorage _handlers;
	public:
		/**
		 * Initializes buffer to 0
		 * @author Cookiemon
		 */
		WindowMessageHandler();
		/**
		 * Checks if the message is related to handled network messages
		 * If yes handle it
		 * @author Cookiemon
		 * @param message Window message
		 * @return true iff message was handled
		 */
		virtual bool pretranslate_message(MSG* message);

	private:
		/**
		 * Reads from a socket and pushes buffer to its MPDMessageHandler
		 * @author Cookiemon
		 * @param sock Socket to read from
		 */
		void ReceiveAndPush(SOCKET sock);
		/**
		 * Accepts a new client on the socket
		 * @author Cookiemon
		 * @param sock Socket where a new client is waiting
		 */
		void AcceptNewClient(SOCKET sock);
	};

}

#endif