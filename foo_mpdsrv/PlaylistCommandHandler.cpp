#include "PlaylistCommandHandler.h"
#include "PlaybackCommands.h"
#include "common.h"
#include "MessageSender.h"
#include <limits>
#include "RequestFromMT.h"

namespace foo_mpdsrv
{
	void HandlePlaylistinfo(MessageSender& caller, std::vector<std::string>& args)
	{
		PlaylistSelector pl = DefaultPlaylistSelector();
		static_api_ptr_t<playlist_manager> man;
		if(args.size() >= 2)
		{
			SongSelector song = SongNumSelector(args[1].c_str());
			metadb_handle_ptr songdata = man->playlist_get_item_handle(pl.GetPlaylistNum(), song.GetSongNum());
			caller.SendSongMetadata(songdata);
		}
		else
		{
			caller.SendPlaylist(pl.GetPlaylistNum());
		}
	}

	void HandlePlchanges(MessageSender& caller, std::vector<std::string>&)
	{
		t_size playlist;
		RequestFromMT mtreq;
		mtreq.RequestPlayingPlaylist(playlist);
		caller.SendPlaylist(playlist);
	}

	void HandleListplaylistinfo(MessageSender& caller, std::vector<std::string>& args)
	{
		if(args.size() < 2)
			throw CommandException(ACK_ERROR_ARG, "not enough arguments");

		PlaylistSelector pl = PlaylistNameSelector(args[1]);
		caller.SendPlaylist(pl.GetPlaylistNum());
	}
}