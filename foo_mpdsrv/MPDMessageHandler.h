#ifndef MPDMESSAGEHANDLER_H
#define MPDMESSAGEHANDLER_H

#include "common.h"
#include "MessageSender.h"
#include "SleepyThread.h"
#include "RWLock.h"
#include <vector>
#include <functional>
#include <unordered_map>
#include <ATLHelpers.h>

namespace foo_mpdsrv
{
	/**
	 * Class used for message relaying
	 * Relays incoming messages to the message handlers
	 * @author Cookiemon
	 */
	class MPDMessageHandler
#ifdef FOO_MPDSRV_THREADED
		: public SleepyThread
#endif
	{
	private:
		// Type for buffer
		typedef std::vector<char> BufferType;
		// Type for message handling routine
		typedef void(*Action)(MessageSender& caller, std::vector<std::string>&);
		// Type for message handler map
		typedef std::unordered_map<std::string, Action> ActionMap;

		// Message sender used for replies
		MessageSender _sender;
		// Buffer for receiving messages
		BufferType _buffer;
		// Command queue for command_list
		// TODO: max commandqueuesize
		std::vector<std::string> _commandQueue;

		// Message handler map
		ActionMap _actions;
		// Pointer to function for handle command routine
		// Alternate implementation would be a state machine
		// for handling simple commands and both command lists
		void (MPDMessageHandler::*_activeHandleCommandRoutine)(const std::string& command);

#ifdef FOO_MPDSRV_THREADED
		// Lock on buffer
		RWLock _bufferLock;
#endif

	private:
		/**
		 * Not copyable
		 * @author Cookiemon
		 */
		MPDMessageHandler(const MPDMessageHandler&);
		/**
		 * Not assignable
		 * @author Cookiemon
		 */
		MPDMessageHandler& operator=(const MPDMessageHandler&);
		
		/**
		 * Wakeup procedure when new data has been
		 * received
		 * @author Cookiemon
		 */
		bool WakeProc(abort_callback &abort);

	public:
		/**
		 * Moveconstructible
		 * @author Cookiemon
		 * @attention right is not in an usable state
		 * after function call
		 * @param right From
		 */
		MPDMessageHandler(MPDMessageHandler&& right);
		/**
		 * Constructs a new message handler
		 * @author Cookiemon
		 * @param transp Transporter that is used for message sending
		 * is taken from
		 */
		explicit MPDMessageHandler(MessageTransporter&& transp);
		/**
		 * Finishes threads if multithreading enabled
		 * @author Cookiemon
		 */
		~MPDMessageHandler();
		
		/**
		 * Class used for message relaying
		 * Relays incoming messages to the message handlers
		 * @author Cookiemon
		 */
		bool IsValid() { return _sender.IsValid(); }
		/**
		 * Returns ID of the used connection
		 * @author Cookiemon
		 */
		SOCKET GetId() { return _sender.GetId(); }
		
		/**
		 * Push received messages into buffer and
		 * relay completed commands to message handlers
		 * @author Cookiemon
		 * @param buf Received bytes
		 * @param numBytes Number of received bytes
		 */
		void PushBuffer(const char* buf, size_t numBytes);
		/**
		 * Relays the current commands in the buffer
		 * @author Cookiemon
		 */
		void HandleBuffer();

	private:
		/**
		 * Executes command according to current message handling routine
		 * @author Cookiemon
		 * @param cmd command to execute (single line of received message)
		 */
		void ExecuteCommand(std::string cmd);
		/**
		 * Splits command into command string and argument strings
		 * @author Cookiemon
		 * @param cmd command to split
		 * @returns List of command and arguments
		 */
		std::vector<std::string> SplitCommand(const std::string& cmd);

		/**
		 * Executes command queue with a given command queue handler
		 * @author Cookiemon
		 * @note Since it's a private function and only exists in
		 * MPDMessageHandler tu the function can be defined there
		 * @tparam T Type of command loop handler
		 * @param CommandLoop Handler of the command list
		 */
		template<typename T>
		void ExecuteCommandQueue(T CommandLoop);
		
		/**
		 * Pushes the command queue until receiving list end command
		 * Sends only one ok for all commands
		 * @param cmd command to execute
		 * @author Cookiemon
		 */
		void HandleCommandListActive(const std::string& cmd);
		/**
		 * Pushes the command queue until receiving list end command
		 * Sends list_OK for every command
		 * @param cmd command to execute
		 * @author Cookiemon
		 */
		void HandleCommandListOkActive(const std::string& cmd);
		/**
		 * Executes a single command and sends OK/ACK
		 * @param cmd command to execute
		 * @author Cookiemon
		 */
		void HandleCommandOnly(const std::string& cmd);
	};

}

#endif