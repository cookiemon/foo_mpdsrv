#ifndef MESSAGESENDER_H
#define MESSAGESENDER_H

#include "common.h"
#include "MessageTransporter.h"
#include <set>

namespace foo_mpdsrv
{
	/**
	 * Class used for message replies
	 * @todo clean up
	 * @todo should only be for sending text not formatting. alternative: extract sending as its own implementation class
	 * @author Cookiemon
	 */
	class MessageSender
	{
	private:
		MessageTransporter _transp;
	public:
		/**
		 * Creates sender that is bound to a
		 * specified transporter which is used
		 * for data transmission
		 * @author Cookiemon
		 * @param connection Connection to bind to
		 */
		explicit MessageSender(MessageTransporter&& transporter);
		/**
		 * Moveconstructs the object
		 * @author Cookiemon
		 * @attention right is invalid after move construction
		 * @param right Object to move
		 */
		MessageSender(MessageSender&& right);

		/**
		 * Sends MPD Welcome Message
		 * @author Cookiemon
		 */
		void SendWelcome();
		/**
		 * Sends Status Message
		 * @author Cookiemon
		 */
		void SendStatus();
		/**
		 * Sends Stats Message
		 * @author Cookiemon
		 */
		void SendStats();
		/**
		 * Sends Metadata of a song in metadb
		 * @author Cookiemon
		 * @param song Metadb of song for which complete
		 * set of metadata should be sent.
		 */
		void SendSongMetadata(metadb_handle_ptr song);
		/**
		 * Sends name of playlist with a given id
		 * @author Cookiemon
		 * @param idx Index of playlist to send
		 */
		void SendPlaylistPath(size_t idx);
		/**
		 * Sends path as directory or filename
		 * @author Cookiemon
		 * @param name Name of path. If path is a directory it should end with '/' or '\\'
		 */
		void SendPath(pfc::string8 name);
		/**
		 * Send output devices. Currently only sends dummy device.
		 * @author Cookiemon
		 * @todo Get real outputs (or don't)
		 */
		void SendOutputs();
		/**
		 * Send playlist content
		 * @author Cookiemon
		 * @param playlist Number of playlist to send
		 */
		void SendPlaylist(t_size playlist);
		/**
		 * Sends OK
		 * @author Cookiemon
		 */
		void SendOk();
		/**
		 * Sends list_OK
		 * @author Cookiemon
		 */
		void SendListOk();
		/**
		 * Sends error message
		 * @author Cookiemon
		 * @param line Line number of command list where error happened
		 * @param cmd Command which caused the error
		 * @param err Error that happened
		 */
		void SendError(unsigned int line, const std::string& cmd, const CommandException& err);

		/**
		 * Close connection
		 * Object is not able to send messages afterwards
		 * @author Cookiemon
		 */
		void CloseConnection();
		/**
		 * Returns if connection is valid, i.e. if connection
		 * is established.
		 * @author Cookiemon
		 */
		bool IsValid() { return _transp.IsValid(); }
		/**
		 * Returns OS specific identifier of connection
		 * @todo no os specific function
		 * @author Cookiemon
		 */
		SOCKET GetId() { return _transp.GetId(); }

	private:
		/**
		 * Translates metadata from file metadata to
		 * mpd expected metadata name
		 */
		pfc::string8 TranslateMetadata(const pfc::string8& data);
		/**
		 * Returns metadata string of a song
		 * @author Cookiemon
		 * @param out String which is used for return value
		 * @param song Metadb of song for which complete
		 * set of metadata should be returned
		 * @param fi Fileinfo of song
		 */
		void GetSongMetadataString(std::string& out, metadb_handle_ptr& song, const file_info& fi);
		/**
		 * Returns metadata string of a song
		 * @author Cookiemon
		 * @param out String which is used for return value
		 * @param song Metadb of song for which complete
		 * set of metadata should be returned
		 * @param fi Fileinfo of song
		 */
		void GetSongMetadataString(std::string& out, metadb_handle_ptr& song);
	};

}

#endif