#include "WindowMessageHandler.h"
#include "common.h"
#include "..\..\foobar2kSDK\pfc\string.h"

namespace foo_mpdsrv
{
	WindowMessageHandler::WindowMessageHandler()
	{
		memset(_buffer, 0, sizeof(_buffer));
	}

	bool WindowMessageHandler::pretranslate_message(MSG* message)
	{
		TRACK_CALL_TEXT("WindowMessageHandler::pretranslate_message()");
		if(message->message == HandledMessage)
		{
			switch(WSAGETSELECTEVENT(message->lParam))
			{
			case FD_ACCEPT:
				{
					MPDMessageHandler newClient(message->wParam);
					if(newClient.IsValid())
					{
						auto& newHandler = _handlers.insert(MessageHandlerStoragePair(newClient.GetId(), std::move(newClient)));
#ifdef FOO_MPDSRV_THREADED
						newHandler.first->second.StartThread();
						SOCKET sock = newHandler.first->second.GetId();
						bool isValid = newHandler.first->second.IsValid();
						bool isActive = newHandler.first->second.IsThreadActive();
						Logger(Logger::FINEST) << "Started thread for Socket " << sock << "(Valid: " << isValid << ",Active:" << isActive << ")";
#endif
					}
				}
				break;
			case FD_CLOSE:
				_handlers.erase(message->wParam);
				break;
			case FD_READ:
				int bytesRec = recv(message->wParam, _buffer, sizeof(_buffer), 0);
				if(bytesRec != SOCKET_ERROR)
				{
					_handlers.find(message->wParam)->second.PushBuffer(_buffer, bytesRec);
				}
				else
				{
					int err = WSAGetLastError();
					Logger(Logger::SEVERE).LogWinError("Could not read from network socket", err);
				}
				break;
			}
			return true;
		}
		return false;
	}

	class InitMessageHandler : public initquit
	{
	private:
		std::auto_ptr<WindowMessageHandler> foo;
	public:
		void on_init()
		{
			foo.reset(new WindowMessageHandler);
		}
		void on_quit()
		{
			foo.release();
		}
	};

	initquit_factory_t<InitMessageHandler> initMessageHandlerFactory;
}