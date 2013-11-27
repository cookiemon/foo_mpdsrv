#ifndef GENERALCOMMANDHANDLER_H
#define GENERALCOMMANDHANDLER_H

#include <string>
#include <vector>

namespace foo_mpdsrv
{
	class MessageSender;

	inline void HandlePing(MessageSender&, std::vector<std::string>&)
	{
	}
	
	void HandleClose(MessageSender& sender, std::vector<std::string>& args);

}

#endif