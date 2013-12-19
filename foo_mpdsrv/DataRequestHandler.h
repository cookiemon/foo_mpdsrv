#ifndef DATAREQUESTHANDLER_H
#define DATAREQUESTHANDLER_H

#include <vector>
#include <string>

namespace foo_mpdsrv
{
	class MessageSender;

	/**
	 * Handles command "currentsong"
	 * Sends the current song
	 * @author Cookiemon
	 * @param MessageSender Connection of the requesting entity
	 * @param args unused
	 */
	void HandleCurrentsong(MessageSender& caller, std::vector<std::string>& args);
	/**
	 * Handles command "lsinfo"
	 * Sends content of directory in database
	 * @author Cookiemon
	 * @param MessageSender Connection of the requesting entity
	 * @param args Directory that is to be listed (default: "/")
	 */
	void HandleLsinfo(MessageSender& caller, std::vector<std::string>& args);
	/**
	 * Handles command "outputs"
	 * Sends output devices (Currently only foobar dummy device)
	 * @todo List real devices (needs functions not available in sdk)
	 * @author Cookiemon
	 * @param MessageSender Connection of the requesting entity
	 * @param args unused
	 */
	void HandleOutputs(MessageSender& caller, std::vector<std::string>& args);
	/**
	 * Handles command "stats"
	 * Sends nothing
	 * @todo Send real statistics (see comment in function)
	 * @author Cookiemon
	 * @param MessageSender Connection of the requesting entity
	 * @param args unused
	 */
	void HandleStats(MessageSender& caller, std::vector<std::string>& args);
}
#endif