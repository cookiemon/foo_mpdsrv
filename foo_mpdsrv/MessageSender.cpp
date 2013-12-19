#include "AutoFreeHandle.h"
#include "MessageSender.h"
#include "PFCExtensions.h"
#include "common.h"
#include "ConfigVars.h"
#include "Converter.h"
#include "RequestFromMT.h"
#include "LibraryConsistencyCheck.h"
#include <sstream>
#include <unordered_map>

namespace foo_mpdsrv
{
	MessageSender::MessageSender(SOCKET connection)
	{
		TRACK_CALL_TEXT("MessageSender::MessageSender()");
		_sock = accept(connection, NULL, NULL);
		if(IsValid())
		{
			SendAnswer(g_MPDGreeting);
		}
	}

	MessageSender::MessageSender(MessageSender&& right)
	{
		TRACK_CALL_TEXT("MessageSender::MessageSender(&&)");
		_sock = right._sock;
		right._sock = static_cast<SOCKET>(SOCKET_ERROR);
	}

	MessageSender::~MessageSender()
	{
		TRACK_CALL_TEXT("MessageSender::~MessageSender()");
		if(_sock != SOCKET_ERROR)
		{
			closesocket(_sock);
		}
	}

	void MessageSender::SendWelcome()
	{
		TRACK_CALL_TEXT("MessageSender::SendWelcome()");
		SendAnswer("OK MPD 0.12.0\n");
	}

	void MessageSender::SendSongMetadata(metadb_handle_ptr song)
	{
		TRACK_CALL_TEXT("MessageSender::SendSongMetadata()");
		std::string str;
		GetSongMetadataString(str, song);
		SendAnswer(str);
	}

	void MessageSender::GetSongMetadataString(std::string& out, metadb_handle_ptr& song)
	{
		file_info_const_impl fi;
		song->get_info(fi);
		GetSongMetadataString(out, song, fi);
	}

	void MessageSender::GetSongMetadataString(std::string& out, metadb_handle_ptr& song, const file_info& fi)
	{
		out.reserve(512);
		static_api_ptr_t<library_manager> lib;
		pfc::string8 path;
		if(lib->get_relative_path(song, path))
		{
			path.replace_char('\\', '/');
			out.append("file: ");
			out.append(path.get_ptr());
			out.append("\n");
		}
		Converter<size_t, std::string> intToStr;
		std::string tmp;
		intToStr(static_cast<size_t>(fi.get_length()), tmp);
		out.append("Time: ");
		out.append(tmp);
		out.append("\n");
		
		t_size numMeta = fi.meta_get_count();
		for(t_size i = 0; i < numMeta; ++i)
		{
			pfc::string8 key = TranslateMetadata(fi.meta_enum_name(i));
			if(key.is_empty())
				continue;

			t_size numMetaValues = fi.meta_enum_value_count(i);
			for(t_size k = 0; k < numMetaValues; ++k)
			{
				pfc::string8 value = fi.meta_enum_value(i, k);
				value.replace_nontext_chars(' ');
				out.append(key.get_ptr());
				out.append(": ");
				out.append(value);
				out.append("\n");
			}
		}
		t_size numInfo = fi.info_get_count();
		for(t_size i = 0; i < numInfo; ++i)
		{
			pfc::string8 key = TranslateMetadata(fi.info_enum_name(i));
			if(key != "")
			{
				pfc::string8 value = fi.info_enum_value(i);
				value.replace_nontext_chars(' ');
				out.append(key.get_ptr());
				out.append(": ");
				out.append(value);
				out.append("\n");
			}
		}
	}

	void MessageSender::SendPlaylist(t_size playlist)
	{
		TRACK_CALL_TEXT("MessageSender::SendPlaylist()");
		static_api_ptr_t<playlist_manager> man;
		pfc::list_t<metadb_handle_ptr> items;
		RequestFromMT req;
		req.RequestPlaylistItems(playlist, items);
		t_size num = items.get_count();
		t_size numNotFound = 0;
		for(t_size i = 0; i < num; ++i)
		{
			try
			{
				std::string songData;
				std::string pos;
				std::string id;
				Converter<t_size, std::string> intToStr;
				intToStr((i - numNotFound), pos);
				{
					FooMetadbHandlePtrLock lock(items[i]);
					const file_info* fi;
					items[i]->get_info_locked(fi);
					GetSongMetadataString(songData, items[i], *fi);
					intToStr(LibraryConsistencyCheck::GetId(items[i], *fi), id);
				}
				songData.append("Pos: ");
				songData.append(pos);
				songData.append("\n");
				songData.append("Id: ");
				songData.append(id);
				songData.append("\n");
				SendAnswer(songData);
			}
			catch(foobar2000_io::exception_io_not_found& e)
			{
				numNotFound += 1;
			}
		}
	}
	
	void MessageSender::SendPath(pfc::string8 name)
	{
		TRACK_CALL_TEXT("MessageSender::SendPath()");
		pfc::string8 msg;
		name.replace_char('\\', '/');
		if(name.ends_with('/'))
		{
			msg.add_string("directory: ");
			name.remove_chars(name.length() - 1, 1);
		}
		else
			msg.add_string("file: ");
		msg.add_string(name);
		msg.add_char('\n');
		SendAnswer(msg);
	}

	void MessageSender::SendPlaylistPath(size_t idx)
	{
		TRACK_CALL_TEXT("MessageSender::SendPlaylistPath()");
		pfc::string8 name;
		RequestFromMT req;
		req.RequestPlaylistName(idx, name);
		pfc::string8 msg("playlist: ");
		msg.add_string(name);
		msg.add_char('\n');
		SendAnswer(msg);
	}

	void MessageSender::SendStatus()
	{
		TRACK_CALL_TEXT("MessageSender::SendStatus()");
		/*TODO:
		xfade: <int seconds> (crossfade in seconds)
		bitrate: <int bitrate> (instantaneous bitrate in kbps)
		audio: <int sampleRate>:<int bits>:<int channels>
		updating_db: <int job id>
		error: if there is an error, returns message here
		nextsong: (next song, playlist song number)
		nextsongid: (next song, playlist songid)[1]
		Questionable:
		song: (current song stopped on or playing, playlist song number)
		songid: (current song stopped on or playing, playlist songid)
		playlist: (31-bit unsigned integer, the playlist version number)
		*/
		std::stringstream out;
		RequestFromMT req;
		bool isPlaying;
		bool isPaused;
		bool itemResult;
		metadb_handle_ptr playing;
		float volume;
		double length;
		double position;
		req.RequestPlaybackInfo(isPlaying, isPaused, itemResult, playing, position, length, volume);

		if(itemResult)
		{
			file_info_impl fileinfo;
			if(playing->get_info(fileinfo))
			{
				out << "bitrate: " << fileinfo.info_get_bitrate() << "\n";
			}
		}
		static_api_ptr_t<playlist_manager> playlistManager;

		volume += 100;
		out << "volume: " << static_cast<int>(volume) << "\n";

		size_t active = playlistManager->playback_order_get_active();
		const std::string curOrder(playlistManager->playback_order_get_name(active));
		const std::string strShuffle("Shuffle");
		bool repeat = (curOrder != "Default");
		bool random = (curOrder == "Random"
			|| (strShuffle.length() < curOrder.length() && std::equal(strShuffle.begin(), strShuffle.end(), curOrder.begin())));
		out << "repeat: " << (repeat ? 1 : 0) << "\n";
		out << "random: " << (random ? 1 : 0) << "\n";


		t_size playlist;
		req.RequestPlayingPlaylist(playlist);
		t_size playlistLength;
		req.RequestPlaylistItemCount(playlist, playlistLength);
		out << "playlistlength: " << playlistLength << "\n";

		std::string state;
		if(isPlaying)
			state = "play";
		else if(isPaused)
			state = "pause";
		else
			state = "stop";
		out << "state: " << state << "\n";

		t_size item;
		if(req.RequestPlayingItemLocation(playlist, item))
			out << "song: " << item << "\n" << "songid: " << item << "\n";

		out << "time: " << static_cast<size_t>(position)
			<< ":" << static_cast<size_t>(length) << "\n";

		out << "playlist: 0\n";
		SendAnswer(out.str());
	}

	void MessageSender::SendStats()
	{
		TRACK_CALL_TEXT("MessageSender::SendStats()");
		/*
		TODO:
		artists: number of artists
		albums: number of albums
		songs: number of songs
		uptime: daemon uptime (time since last startup) in seconds
		playtime: time length of music played
		db_playtime: sum of all song times in db
		db_update: last db update in UNIX time
		*/
		std::stringstream out;
		static_api_ptr_t<library_manager> libraryManager;
	}
	
	void MessageSender::SendOutputs()
	{
		TRACK_CALL_TEXT("MessageSender::SendOutputs()");
		SendAnswer("outputid: 0\noutputname: foobar\noutputenabled: 1\n");
	}

	void MessageSender::SendOk()
	{
		TRACK_CALL_TEXT("MessageSender::SendOk()");
		SendAnswer("OK\n");
	}

	void MessageSender::SendListOk()
	{
		TRACK_CALL_TEXT("MessageSender::SendListOk()");
		SendAnswer("list_OK\n");
	}

	void MessageSender::CloseConnection()
	{
		TRACK_CALL_TEXT("MessageSender::CloseConnection()");
		closesocket(_sock);
		_sock = SOCKET_ERROR;
	}

	void MessageSender::SendError(unsigned int line, const std::string& command, const CommandException& err)
	{
		TRACK_CALL_TEXT("MessageSender::SendError()");
		std::stringstream ans;
		ans << "ACK [" << err.GetError() << '@' << line << "] {" << command << "} " << err.what() << '\n';
		SendAnswer(ans.str());
	}

	bool MessageSender::SendAnswer(const std::string& message)
	{
		size_t numBytes = message.length();
		return SendBytes(message.c_str(), numBytes);
	}

	bool MessageSender::SendAnswer(const pfc::string8& message)
	{
		size_t numBytes = message.get_length();
		return SendBytes(message.get_ptr(), numBytes);
	}

	bool MessageSender::SendAnswer(std::istream& message)
	{
		std::string buf;
		bool retVal = true;
		while(std::getline(message, buf))
			retVal = retVal && SendAnswer(buf + '\n');
		return retVal;
	}

	bool MessageSender::SendAnswer(const char* message)
	{
		size_t numBytes = strlen(message);
		return SendBytes(message, numBytes);
	}

	bool MessageSender::SendBytes(const char* buf, int numBytes)
	{
		if(_sock == static_cast<SOCKET>(SOCKET_ERROR)) { return false; }
		if(numBytes == 0) { return true; }

		WaitForSocket();
		
		int bytesSend = send(_sock, buf, numBytes, 0);
		if(bytesSend == static_cast<SOCKET>(SOCKET_ERROR))
		{
			int lastError = WSAGetLastError();
			if(lastError != WSAEWOULDBLOCK)
			{
				Logger(Logger::SEVERE).LogWinError("Could not send message", lastError);
				return false;
			}
			else
			{
				Logger(Logger::SEVERE) << "send returned WSAEWOULDBLOCK";
			}
		}
		else if(numBytes != bytesSend)
		{
			Logger(Logger::SEVERE) << "Not all bytes were sent";
			return false;
		}
		Logger(Logger::FINEST).Log("O: ").Write(buf, numBytes);
		return true;
	}

	void MessageSender::WaitForSocket()
	{
		int lastError = 0;
		fd_set fds;
		fds.fd_count = 1;
		fds.fd_array[0] = _sock;
		lastError = select(0, NULL, &fds, NULL, NULL);
		if(lastError == SOCKET_ERROR)
		{
			lastError = WSAGetLastError();
			Logger(Logger::SEVERE).LogWinError("Could not select socket", lastError);
		}
	}

	typedef std::unordered_map<pfc::string8, pfc::string8> PfcStdStringDict;
	typedef std::pair<pfc::string8, pfc::string8> PfcStdStringPair;

	static PfcStdStringDict InitDict()
	{
		TRACK_CALL_TEXT("MessageSender::InitDict()");
		PfcStdStringDict dict;
		dict.insert(PfcStdStringPair("file", "file"));
		dict.insert(PfcStdStringPair("artist", "Artist"));
		dict.insert(PfcStdStringPair("album", "Album"));
		dict.insert(PfcStdStringPair("album artist", "AlbumArtist"));
		dict.insert(PfcStdStringPair("title", "Title"));
		dict.insert(PfcStdStringPair("track", "Track"));
		dict.insert(PfcStdStringPair("date", "Date"));
		dict.insert(PfcStdStringPair("genre", "Genre"));
		dict.insert(PfcStdStringPair("disc", "Disc"));
		dict.insert(PfcStdStringPair("pos", "Pos"));
//		dict.insert(PfcStdStringPair(g_IdString, "Id"));
		return dict;
	}

	pfc::string8 MessageSender::TranslateMetadata(const pfc::string8& meta)
	{
		static PfcStdStringDict fooToMpdDict = InitDict();
		const auto& mpdMeta = fooToMpdDict.find(meta);
		if(mpdMeta == fooToMpdDict.end())
			return "";
		else
			return mpdMeta->second;
	}
}