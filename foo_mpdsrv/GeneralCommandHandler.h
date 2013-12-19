#ifndef GENERALCOMMANDHANDLER_H
#define GENERALCOMMANDHANDLER_H

#include <string>
#include <vector>

namespace foo_mpdsrv
{
	class MessageSender;

	/**
	 * Handles command "ping"
	 * Only sends automatically generated OK
	 * @author Cookiemon
	 * @param MessageSender Connection of the requesting entity
	 * @param args unused
	 */
	inline void HandlePing(MessageSender&, std::vector<std::string>&)
	{
	}
	
	/**
	 * Handles command "close"
	 * Closes the connection. Afterwards MessageSender is in
	 * a dummy state that does nothing instead of sending messages
	 * @author Cookiemon
	 * @param MessageSender Connection of the requesting entity
	 * @param args unused
	 */
	void HandleClose(MessageSender& sender, std::vector<std::string>& args);

}

#endif