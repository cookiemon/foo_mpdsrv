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

	/**
	 * Handles command "play"
	 * Starts playback of a song from the default playlist
	 * @author Cookiemon
	 * @param MessageSender Connection of the requesting entity
	 * @param args Position of song
	 */
	void HandlePlay(MessageSender&, std::vector<std::string>& args);
	/**
	 * Handles command "playid"
	 * Starts playback of a song with a given id
	 * @author Cookiemon
	 * @param MessageSender Connection of the requesting entity
	 * @param args Id of the song from the current playlist
	 */
	void HandlePlayId(MessageSender&, std::vector<std::string>& args);
	/**
	 * Handles command "pause"
	 * Switches pause status
	 * @author Cookiemon
	 * @param MessageSender Connection of the requesting entity
	 * @param args 1 if pausing, 0 if unpausing, Empty if toggling between both
	 */
	void HandlePause(MessageSender&, std::vector<std::string>& args);
	/**
	 * Handles command "stop"
	 * Stops current playback
	 * @author Cookiemon
	 * @param MessageSender Connection of the requesting entity
	 * @param args unused
	 */
	void HandleStop(MessageSender&, std::vector<std::string>& args);
	/**
	 * Handles command "next"
	 * Starts playback of the next song in play order
	 * @author Cookiemon
	 * @param MessageSender Connection of the requesting entity
	 * @param args unused
	 */
	void HandleNext(MessageSender&, std::vector<std::string>& args);
	/**
	 * Handles command "prev"
	 * Starts playback of the last song in play order
	 * @bug If random playback stops playback instead
	 * @author Cookiemon
	 * @param MessageSender Connection of the requesting entity
	 * @param args unused
	 */
	void HandlePrevious(MessageSender&, std::vector<std::string>& args);
	/**
	 * Handles command "status"
	 * Sends current playback status. Includes pause status, bitrate etc.
	 * @todo Not all status fields are sent (see comment in function)
	 * @author Cookiemon
	 * @param MessageSender Connection of the requesting entity
	 * @param args unused
	 */
	void HandleStatus(MessageSender& caller, std::vector<std::string>& args);
	/**
	 * Handles command "volume"
	 * Sets new volume
	 * @todo Maybe handle +X/-X as parameter
	 * @author Cookiemon
	 * @param MessageSender Connection of the requesting entity
	 * @param args 0-100 new volume level
	 */
	void HandleVolume(MessageSender& caller, std::vector<std::string>& args);
}

#endif