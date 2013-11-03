#include "MessageSender.h"
#include "common.h"
#include "ConfigVars.h"
#include "RequestFromMT.h"
#include "LibraryConsistencyCheck.h"
#include <sstream>
#include <unordered_map>

//TODO: IMPROVE!
inline void popupNetworkError(const char* message, int errorNum = -1)
{
	if(errorNum == -1)
		errorNum = WSAGetLastError();
	//console::formatter form;
	//form << message << " (" << errorNum << ", " << gai_strerrorA(errorNum) << ")";
	//popup_message::g_show(form, PLUGINNAME, popup_message::icon_error);
	std::ofstream err("C:\\logs\\foo_mpd.err");
	err << message << " (" << errorNum << ", " << gai_strerrorA(errorNum) << ")" << std::endl;
}

namespace foo_mpdsrv
{
	MessageSender::MessageSender(SOCKET connection)
	{
		_sock = accept(connection, NULL, NULL);
		if(IsValid())
		{
			SendAnswer("OK MPD 0.12.2\n");
		}
	}

	MessageSender::MessageSender(MessageSender&& right)
	{
		_sock = right._sock;
		right._sock = static_cast<SOCKET>(SOCKET_ERROR);
	}

	MessageSender::~MessageSender()
	{
		if(_sock != SOCKET_ERROR)
		{
			closesocket(_sock);
		}
	}

	void MessageSender::SendWelcome()
	{
		SendAnswer("OK MPD 0.12.0\n");
	}

	void MessageSender::SendSongMetadata(metadb_handle_ptr song)
	{
 		std::stringstream out;
		static_api_ptr_t<library_manager> lib;

		pfc::string8 path;
		if(lib->get_relative_path(song, path))
		{
			path.replace_char('\\', '/');
			out << "file: " << path.get_ptr() << "\n";
		}
		
		//t_filestats stats = song->get_filestats();
		file_info_const_impl fi;
		if(song->get_info(fi))
		{
			out << "Time: " << static_cast<int>(fi.get_length()) << "\n";

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
					out << key.get_ptr() << ": " << value << "\n";
				}
			}
			t_size numInfo = fi.info_get_count();
			for(t_size i = 0; i < numInfo; ++i)
			{
				pfc::string8 key = TranslateMetadata(fi.info_enum_name(i));
				pfc::string8 value = fi.info_enum_value(i);
				value.replace_nontext_chars(' ');
				out << key.get_ptr() << ": " << value << "\n";
			}
		}
		SendAnswer(out.str());
	}

	void MessageSender::SendPlaylist(t_size playlist)
	{
		static_api_ptr_t<playlist_manager> man;
		pfc::list_t<metadb_handle_ptr> items;

		man->playlist_get_all_items(playlist, items);
		t_size num = items.get_count();
		for(t_size i = 0; i < num; ++i)
		{
			SendSongMetadata(items[i]);
			std::stringstream str;
			str << "Pos: " << i << "\n";
			str << "Id: " << LibraryConsistencyCheck::GetId(items[i]) << "\n";
			SendAnswer(str.str());
		}
	}
	
	void MessageSender::SendPath(pfc::string8 name)
	{
		pfc::string8 msg;
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
		static_api_ptr_t<playlist_manager> plman;
		pfc::string8 name;
		plman->playlist_get_name(idx, name);
		pfc::string8 msg("playlist: ");
		msg.add_string(name);
		msg.add_char('\n');
		SendAnswer(msg);
	}

	void MessageSender::SendStatus()
	{
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
		static_api_ptr_t<playback_control> playback;
		static_api_ptr_t<playlist_manager> playlistManager;
		metadb_handle_ptr playing;
		if(playback->get_now_playing(playing))
		{
			file_info_impl fileinfo;
			if(playing->get_info(fileinfo))
			{
				out << "bitrate: " << fileinfo.info_get_bitrate() << "\n";
			}
		}

		int volume = static_cast<int>(playback->get_volume());
		volume += 100;
		out << "volume: " << volume << "\n";

		size_t active = playlistManager->playback_order_get_active();
		const std::string curOrder(playlistManager->playback_order_get_name(active));
		const std::string strShuffle("Shuffle");
		bool repeat = (curOrder != "Default");
		bool random = (curOrder == "Random"
			|| (strShuffle.length() < curOrder.length() && std::equal(strShuffle.begin(), strShuffle.end(), curOrder.begin())));
		out << "repeat: " << (repeat?1:0) << "\n";
		out << "random: " << (random?1:0) << "\n";

		int playlistLength = playlistManager->activeplaylist_get_item_count();
		out << "playlistlength: " << playlistLength << "\n";

		std::string state;
		if(playback->is_playing())
			state = "play";
		else if(playback->is_paused())
			state = "pause";
		else
			state = "stop";
		out << "state: " << state << "\n";

		t_size playlist, item;
		if(playlistManager->get_playing_item_location(&playlist, &item))
			out << "song: " << item << "\n" << "songid: " << item << "\n";

		out << "time: " << static_cast<int>(playback->playback_get_position())
			<< ":" << static_cast<int>(playback->playback_get_length()) << "\n";

		out << "playlist: 0\n";
		SendAnswer(out.str());
	}

	void MessageSender::SendStats()
	{
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
		SendAnswer("outputid: 0\noutputname: foobar\noutputenabled: 1\nOK\n");
	}

	void MessageSender::SendOk()
	{
		SendAnswer("OK\n");
	}

	void MessageSender::SendListOk()
	{
		SendAnswer("list_OK\n");
	}

	void MessageSender::SendError(unsigned int line, const std::string& command, const CommandException& err)
	{
		std::stringstream ans;
		ans << "ACK [" << err.GetError() << '@' << line << "] {" << command << "} " << err.what() << '\n';
		SendAnswer(ans.str());
	}

	bool MessageSender::SendAnswer(const std::string& message)
	{
		int numBytes = message.length();
		return SendBytes(message.c_str(), numBytes);
	}

	bool MessageSender::WakeProc(abort_callback& p_abort)
	{
		return true;
	}

	bool MessageSender::SendAnswer(const pfc::string8& message)
	{
		int numBytes = message.get_length();
#ifdef _DEBUG
		std::ofstream log;
		log.open("C:\\logs\\foo_mpd.log", std::ofstream::out | std::ofstream::app);
		log << "O: " << message << std::endl;
		log.close();
#endif
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
		int numBytes = strlen(message);
		return SendBytes(message, numBytes);
	}

	bool MessageSender::SendBytes(const char* buf, int numBytes)
	{
		if(numBytes == 0) { return true; }

		int bytesSend = send(_sock, buf, numBytes, 0);
		if(bytesSend == static_cast<SOCKET>(SOCKET_ERROR))
		{
			if(WSAGetLastError() != WSAEWOULDBLOCK)
			{
				popupNetworkError("Could not send message");
				return false;
			}
		}
		else if(numBytes != bytesSend)
		{
			popup_message::g_show("Could not send all bytes. Investigate!", PLUGINNAME, popup_message::icon_error);
			return false;
		}

#ifdef _DEBUG
			{
				std::ofstream log;
				log.open("C:\\logs\\foo_mpd.log", std::ofstream::out | std::ofstream::app);
				log << "O: ";
				log.write(buf, numBytes);
				log.flush();
				log.close();
			}
#endif
		return true;
	}

	class PfcHash
	{
public:
	size_t operator()(const pfc::string8& _Keyval) const
		{	// hash _Keyval to size_t value by pseudorandomizing transform
		size_t _Val = 2166136261U;
		size_t _First = 0;
		size_t _Last = _Keyval.length();
		size_t _Stride = 1 + _Last / 10;

		for(; _First < _Last; _First += _Stride)
			_Val = 16777619U * _Val ^ (size_t)_Keyval[_First];
		return (_Val);
		}
	};

	typedef std::unordered_map<pfc::string8, pfc::string8, PfcHash> PfcStdStringDict;
	typedef std::pair<pfc::string8, pfc::string8> PfcStdStringPair;

	PfcStdStringDict initDict()
	{
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
		dict.insert(PfcStdStringPair(g_IdString, "Id"));
		return dict;
	}

	pfc::string8 MessageSender::TranslateMetadata(const pfc::string8& meta)
	{
		static PfcStdStringDict fooToMpdDict = initDict();
		const auto& mpdMeta = fooToMpdDict.find(meta);
		if(mpdMeta == fooToMpdDict.end())
			return "";
		else
			return mpdMeta->second;
	}
}