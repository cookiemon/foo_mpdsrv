#ifndef MESSAGETRANSPORTER_H
#define MESSAGETRANSPORTER_H

#include "common.h"

namespace foo_mpdsrv
{
	/**
	 * Transports message to destination.
	 * Currently only network connection via WSA
	 * are supported but might be abstracted for
	 * other transportation methods (e.g. Pipes)
	 * @author Cookiemon
	 */
	class MessageTransporter
	{
	private:
		SOCKET _sock;
	private:
		// Noncopyable
		MessageTransporter(MessageTransporter&);
		// Nonassignable
		MessageTransporter operator=(MessageTransporter&);
	public:
		/**
		 * Initializes transporter with invalid socket
		 * @author Cookiemon
		 */
		MessageTransporter();
		/**
		 * Initializes transporter with a socket accepted
		 * from connection
		 * @author Cookiemon
		 * @param connection Socket from which initialization
		 * socket is taken
		 */
		MessageTransporter(SOCKET connection);
		/**
		 * Moveconstructor
		 * @author Cookiemon
		 * @param right Moveargument
		 */
		MessageTransporter(MessageTransporter&& right);
		/**
		 * Uninitializes and closes socket if valid
		 * @author Cookiemon
		 */
		~MessageTransporter();

		/**
		 * Sends a message
		 * @author Cookiemon
		 * @param answ Message to send
		 * @return true iff sending successful.
		 */
		bool SendAnswer(const std::string& answ);
		/**
		 * Sends a message
		 * @author Cookiemon
		 * @param answ Message to send
		 * @return true iff sending successful.
		 */
		bool SendAnswer(const pfc::string8& answ);
		/**
		 * Sends a message
		 * @author Cookiemon
		 * @param answ Message to send (0-terminated C-string)
		 * @return true iff sending successful.
		 */
		bool SendAnswer(const char* answ);
		/**
		 * Sends a message
		 * @author Cookiemon
		 * @param answ Message to send
		 * @return true iff sending successful.
		 */
		bool SendAnswer(std::istream& answ);
		/**
		 * Sends a bunch of bytes
		 * @author Cookiemon
		 * @param answ Message to send
		 * @param numBytes Number of bytes to send
		 * @return true iff sending successful.
		 */
		bool SendBytes(const char* buf, int numBytes);
		/**
		 * Wait until transporter is available for writing
		 * @author Cookiemon
		 */
		void WaitUntilReady();
		/**
		 * Returns if connection is valid, i.e. if connection
		 * is established.
		 * @author Cookiemon
		 */
		bool IsValid() { return _sock != SOCKET_ERROR; }
		/**
		 * Returns OS specific identifier of connection
		 * @todo no os specific function
		 * @author Cookiemon
		 */
		SOCKET GetId() { return _sock; }
		/**
		 * Closes and invalidates the transport
		 * @author Cookiemon
		 */
		void Close();
	};
}

#endif