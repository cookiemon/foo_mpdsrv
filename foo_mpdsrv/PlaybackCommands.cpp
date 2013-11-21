#include "PlaybackCommands.h"
#include "LibraryConsistencyCheck.h"
#include "RequestFromMT.h"

namespace foo_mpdsrv
{
	SongSelector::SongSelector()
	{
		_songNum = std::numeric_limits<t_size>::max();
	}
	t_size SongSelector::GetSongNum()
	{
		return _songNum;
	}

	SongNumSelector::SongNumSelector(t_size songNum)
	{
		_songNum = songNum;
	}
	SongNumSelector::SongNumSelector(const char* songNum)
	{
		_songNum = ConvertToLong(songNum);
	}
	SongNumSelector::SongNumSelector(const std::string& songNum)
	{
		_songNum = ConvertToLong(songNum.c_str());
	}


	SongIdSelector::HandleIdSearch::HandleIdSearch(long searchedItem)
	{
		_searchedItem = searchedItem;
		_pos = std::numeric_limits<t_size>::max();
	}
	bool SongIdSelector::HandleIdSearch::on_item(t_size p_index,const metadb_handle_ptr & p_location,bool b_selected)
	{
		if(LibraryConsistencyCheck::GetId(p_location) == _searchedItem)
		{
			_pos = p_index;
			return false;
		}
		return true;
	}
	t_size SongIdSelector::HandleIdSearch::GetResult()
	{
		return _pos;
	}
	SongIdSelector::SongIdSelector(long id, t_size playlist)
	{
		InitId(id, playlist);
	}
	SongIdSelector::SongIdSelector(const char* id, t_size playlist)
	{
		InitId(ConvertToLong(id), playlist);
	}
	SongIdSelector::SongIdSelector(const std::string& id, t_size playlist)
	{
		InitId(ConvertToLong(id.c_str()), playlist);
	}

	void SongIdSelector::InitId(long id, t_size playlist)
	{
		HandleIdSearch searchHandler(id);
		static_api_ptr_t<playlist_manager> plMan;
		plMan->playlist_enum_items(playlist, searchHandler, bit_array_true());
		_songNum = searchHandler.GetResult();
	}

	PlaylistSelector::PlaylistSelector()
	{
		_plNum = std::numeric_limits<t_size>::max();
	}
	t_size PlaylistSelector::GetPlaylistNum()
	{
		return _plNum;
	}

#include "RequestFromMT.h"

	DefaultPlaylistSelector::DefaultPlaylistSelector()
	{
		RequestFromMT req;
		req.RequestPlayingPlaylist(_plNum);
		if(_plNum == std::numeric_limits<t_size>::max())
			req.RequestActivePlaylist(_plNum);
		/*static_api_ptr_t<playlist_manager> man;
		_plNum = man->get_playing_playlist();
		if(_plNum == std::numeric_limits<t_size>::max())
			_plNum = man->get_active_playlist();*/
	}

	PlaylistNameSelector::PlaylistNameSelector(const char* name)
	{
		Select(pfc::string8(name));
	}
	PlaylistNameSelector::PlaylistNameSelector(const std::string& name)
	{
		Select(pfc::string8(name.c_str()));
	}
	PlaylistNameSelector::PlaylistNameSelector(const pfc::string_base& name)
	{
		Select(name);
	}

	void PlaylistNameSelector::Select(const pfc::string_base& nameToFind)
	{
		RequestFromMT req;
		req.DoCallback([&](){
			static_api_ptr_t<playlist_manager> plman;
			t_size num = plman->get_playlist_count();

			for(t_size i = 0; i < num; ++i)
			{
				pfc::string8 name;
				plman->playlist_get_name(i, name);
				if(nameToFind == name)
				{
					_plNum = i;
					return;
				}
			}
		});
	}

	void PlayItemMTCallback(PlaylistSelector pl, SongSelector song)
	{
		static_api_ptr_t<playlist_manager> man;
		t_size songNum = song.GetSongNum();
		if(songNum != std::numeric_limits<t_size>::max())
		{
			man->playlist_execute_default_action(pl.GetPlaylistNum(), song.GetSongNum());
		}
		else
		{
			static_api_ptr_t<playback_control> control;
			if(control->is_paused())
				control->pause(false);
			else
				control->start();
		}
	}

	void PlayItem(PlaylistSelector pl, SongSelector song)
	{
		RequestFromMT req;
		req.DoCallback([=](){ PlayItemMTCallback(pl, song); });
	}
}