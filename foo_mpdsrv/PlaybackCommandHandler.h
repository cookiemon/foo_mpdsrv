#ifndef PLAYBACKCOMMANDHANDLER_H
#define PLAYBACKCOMMANDHANDLER_H

#include <vector>

namespace foo_mpdsrv
{
	class MessageSender;
	namespace PlayState
	{
		enum PlayState
		{
			Play,
			Pause,
			Stop
		};
	}
	
	void HandlePlay(MessageSender&, std::vector<std::string>& args);
	void HandlePlayId(MessageSender&, std::vector<std::string>& args);
	void HandlePause(MessageSender&, std::vector<std::string>& args);
	void HandlePlay(MessageSender&, std::vector<std::string>& args);
	void HandleStop(MessageSender&, std::vector<std::string>& args);
	void HandleNext(MessageSender&, std::vector<std::string>& args);
	void HandlePrevious(MessageSender&, std::vector<std::string>& args);
	void HandleStatus(MessageSender& caller, std::vector<std::string>& args);
	void HandleVolume(MessageSender& caller, std::vector<std::string>& args);
}

#endif