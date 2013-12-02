#ifndef MESSAGESENDER_H
#define MESSAGESENDER_H

#include "common.h"
#include <set>

namespace foo_mpdsrv
{

	class MessageSender
	{
	private:
		SOCKET _sock;
	public:
		MessageSender(SOCKET connection);
		MessageSender(MessageSender&& right);
		~MessageSender();
	
		void SendWelcome();

		void SendStatus();
		void SendStats();
		void SendSongMetadata(metadb_handle_ptr song);
		std::string MessageSender::GetSongMetadataString(metadb_handle_ptr& song);
		void SendPlaylistPath(size_t idx);
		void SendPath(pfc::string8 name);
		//void SendFilename(metadb_handle_ptr song);
		void SendOutputs();

		void SendPlaylist(t_size playlist);

		void SendOk();
		void SendListOk();
		void SendError(unsigned int line, const std::string& cmd, const CommandException& err);

		void CloseConnection();

		bool IsValid() { return _sock != SOCKET_ERROR; }
		SOCKET GetId() { return _sock; }

	protected:
		bool WakeProc(abort_callback& p_abort);
	private:
		bool SendAnswer(const std::string& answ);
		bool SendAnswer(const pfc::string8& answ);
		bool SendAnswer(const char* answ);
		bool SendAnswer(std::istream& answ);
		bool SendBytes(const char* buf, int numBytes);
		pfc::string8 TranslateMetadata(const pfc::string8& data);
	};

}

#endif