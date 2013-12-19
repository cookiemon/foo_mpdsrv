#ifndef MESSAGESENDER_H
#define MESSAGESENDER_H

#include "common.h"
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
		SOCKET _sock;
	public:
		/**
		 * Creates sender that is bound to the next
		 * accepted connection on the given socket
		 * @author Cookiemon
		 * @param connection Connection to bind to
		 */
		explicit MessageSender(SOCKET connection);
		/**
		 * Moveconstructs the object
		 * @author Cookiemon
		 * @attention right is invalid after move construction
		 * @param right Object to move
		 */
		MessageSender(MessageSender&& right);
		/**
		 * Closes the bound socket
		 * @author Cookiemon
		 */
		~MessageSender();

		// ---- No documentation since rewrite probably incoming
		void SendWelcome();
		void SendStatus();
		void SendStats();
		void SendSongMetadata(metadb_handle_ptr song);
		void GetSongMetadataString(std::string& out, metadb_handle_ptr& song, const file_info& fi);
		void GetSongMetadataString(std::string& out, metadb_handle_ptr& song);
		void SendPlaylistPath(size_t idx);
		void SendPath(pfc::string8 name);
		void SendOutputs();
		void SendPlaylist(t_size playlist);
		void SendOk();
		void SendListOk();
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
		bool IsValid() { return _sock != SOCKET_ERROR; }
		/**
		 * Returns OS specific identifier of connection
		 * @todo no os specific function
		 * @author Cookiemon
		 */
		SOCKET GetId() { return _sock; }

	private:
		// ---- No documentation because rewrite is probably coming up
		// ---- Will either be future public interface of MessageSender
		// ---- Or future public interface of <someotherclass>
		bool SendAnswer(const std::string& answ);
		bool SendAnswer(const pfc::string8& answ);
		bool SendAnswer(const char* answ);
		bool SendAnswer(std::istream& answ);
		bool SendBytes(const char* buf, int numBytes);
		void WaitForSocket();
		pfc::string8 TranslateMetadata(const pfc::string8& data);
	};

}

#endif