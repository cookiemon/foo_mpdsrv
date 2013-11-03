#include "PlaylistCommandHandler.h"
#include "common.h"
#include "MessageSender.h"
#include <limits>
#include "RequestFromMT.h"

namespace foo_mpdsrv
{
	void HandlePlaylistinfo(MessageSender& caller, std::vector<std::string>& args)
	{
		static_api_ptr_t<playlist_manager> man;
		t_size playlist = man->get_playing_playlist();
		if(playlist == std::numeric_limits<t_size>::max())
			playlist = man->get_active_playlist();
		if(args.size() >= 2)
		{
			long numpos = ConvertToLong(args[1].c_str());
			metadb_handle_ptr songdata = man->playlist_get_item_handle(playlist, numpos);
			caller.SendSongMetadata(songdata);
		}
		else
		{
			caller.SendPlaylist(playlist);
		}
	}

	void HandlePlchanges(MessageSender& caller, std::vector<std::string>&)
	{
		static_api_ptr_t<playlist_manager> man;
		t_size playlist = man->get_playing_playlist();
		if(playlist == std::numeric_limits<t_size>::max())
			playlist = man->get_active_playlist();
		caller.SendPlaylist(playlist);
	}

	void HandleListplaylistinfo(MessageSender& caller, std::vector<std::string>& args)
	{
		if(args.size() < 2)
			throw CommandException(ACK_ERROR_ARG, "not enough arguments");

		static_api_ptr_t<playlist_manager> man;
		t_size num = man->get_playlist_count();
		for(t_size i = 0; i < num; ++i)
		{
			pfc::string8 name;
			man->playlist_get_name(i, name);
			if(args[0] == name.get_ptr())
			{
				caller.SendPlaylist(i);
				return;
			}
		}
	}
}