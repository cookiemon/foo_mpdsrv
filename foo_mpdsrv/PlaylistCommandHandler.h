#ifndef PLAYLISTCOMMANDHANDLER_H
#define PLAYLISTCOMMANDHANDLER_H

#include <vector>

namespace foo_mpdsrv
{
	class MessageSender;
	
	void HandlePlaylistinfo(MessageSender& caller, std::vector<std::string>&);
	void HandlePlchanges(MessageSender& caller, std::vector<std::string>&);
	void HandleListplaylistinfo(MessageSender& caller, std::vector<std::string>&);
}

#endif