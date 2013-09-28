#ifndef GENERALCOMMANDHANDLER_H
#define GENERALCOMMANDHANDLER_H

#include <vector>

namespace foo_mpdsrv
{
	class MessageSender;

	inline void HandlePing(MessageSender&, std::vector<std::string>&)
	{
	}

}

#endif