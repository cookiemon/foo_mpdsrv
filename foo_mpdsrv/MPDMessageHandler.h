#ifndef MPDMESSAGEHANDLER_H
#define MPDMESSAGEHANDLER_H

#include "common.h"
#include "MessageSender.h"
#include "SleepyThread.h"
#include <sstream>
#include <vector>
#include <functional>
#include <unordered_map>
#include <ATLHelpers.h>

namespace foo_mpdsrv
{

	class MPDMessageHandler
#ifdef FOO_MPDSRV_THREADED
		: public SleepyThread
#endif
	{
	private:
		MessageSender _sender;
		std::string _lastIncomplete;
		std::stringstream _buffer;
		bool _accumulateList;
		bool _list_OK;
		std::vector<std::string> _commandQueue;

		typedef void(*Action)(MessageSender& caller, std::vector<std::string>&);
		typedef std::unordered_map<std::string, Action> ActionMap;
		ActionMap _actions;

	private:
		MPDMessageHandler();
		MPDMessageHandler(const MPDMessageHandler&);
		MPDMessageHandler& operator=(const MPDMessageHandler&);

		bool WakeProc(abort_callback &abort);

	public:
		MPDMessageHandler(MPDMessageHandler&& right);
		explicit MPDMessageHandler(SOCKET connection);
		~MPDMessageHandler();
		
		bool IsValid() { return _sender.IsValid(); }
		SOCKET GetId() { return _sender.GetId(); }

		void PushBuffer(const char* buf, size_t numBytes);
		void HandleBuffer();
		void Shutdown();

	private:

		void ExecuteCommand(std::string cmd);
		void ExecuteCommandQueue(bool respondAfterEvery);
		std::vector<std::string> SplitCommand(const std::string& cmd);
	};

}

#endif