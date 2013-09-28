#ifndef WINDOWMESSAGEHANDLER_H
#define WINDOWMESSAGEHANDLER_H

#include "common.h"
#include "MPDMessageHandler.h"
#include <map>

namespace foo_mpdsrv
{

	class WindowMessageHandler : public message_filter_impl_base
	{
	public:
		static const int HandledMessage = WM_USER + 1;

	private:
		typedef std::map<SOCKET, MPDMessageHandler> MessageHandlerStorage;
		typedef std::pair<SOCKET, MPDMessageHandler> MessageHandlerStoragePair;
		char _buffer[256];
		MessageHandlerStorage _handlers;
	public:
		WindowMessageHandler();

		bool pretranslate_message(MSG* message);
	};

}

#endif