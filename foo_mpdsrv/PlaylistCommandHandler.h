#ifndef PLAYLISTCOMMANDHANDLER_H
#define PLAYLISTCOMMANDHANDLER_H

#include <vector>

namespace foo_mpdsrv
{
	class MessageSender;
	
	/**
	 * Handles command "playlistinfo"
	 * Sends a song from the current playlist
	 * @author Cookiemon
	 * @param MessageSender Connection of the requesting entity
	 * @param args Position of the song to send (Default: Sends all songs)
	 */
	void HandlePlaylistinfo(MessageSender& caller, std::vector<std::string>&);
	/**
	 * Handles command "plchanges"
	 * Sends changes of a playlist since a specified version
	 * @todo Versioning (lolnope)
	 * @author Cookiemon
	 * @param MessageSender Connection of the requesting entity
	 * @param args 0-INT_MAX version number of last changes to include
	 */
	void HandlePlchanges(MessageSender& caller, std::vector<std::string>&);
	/**
	 * Handles command "listplaylistinfo"
	 * Sends a playlist
	 * @author Cookiemon
	 * @param MessageSender Connection of the requesting entity
	 * @param args Name of the playlist to send
	 */
	void HandleListplaylistinfo(MessageSender& caller, std::vector<std::string>&);
}

#endif