#ifndef PLAYBACKCOMMANDS_H
#define PLAYBACKCOMMANDS_H

#include "common.h"
#include "ConfigVars.h"

namespace foo_mpdsrv
{
	class SongSelector
	{
	protected:
		t_size _songNum;
	public:
		SongSelector();
		t_size GetSongNum();
	};

	class SongNumSelector : public SongSelector
	{
	public:
		explicit SongNumSelector(t_size songNum);
		explicit SongNumSelector(const char* songNum);
		explicit SongNumSelector(const std::string& songNum);
	};

	class SongIdSelector : public SongSelector
	{
	private:
		class HandleIdSearch : public playlist_manager::enum_items_callback
		{
		private:
			idtype _searchedItem;
			t_size _pos;
		public:
			explicit HandleIdSearch(idtype searchedItem);
			virtual bool on_item(t_size p_index,const metadb_handle_ptr & p_location,bool b_selected);
			t_size GetResult();
		};
	public:
		SongIdSelector(idtype id, t_size playlist);
	private:
		void InitId(idtype id, t_size playlist);
	};

	class PlaylistSelector
	{
	protected:
		t_size _plNum;
	public:
		PlaylistSelector();
		t_size GetPlaylistNum();
	};

	class DefaultPlaylistSelector : public PlaylistSelector
	{
	public:
		DefaultPlaylistSelector();
	};

	class PlaylistNameSelector : public PlaylistSelector
	{
	public:
		PlaylistNameSelector(const char* name);
		PlaylistNameSelector(const std::string& name);
		PlaylistNameSelector(const pfc::string_base& name);
		void Select(const pfc::string_base& name);
	};

	void PlayItem(PlaylistSelector pl, SongSelector song);
}

#endif