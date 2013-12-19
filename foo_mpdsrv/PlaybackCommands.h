#ifndef PLAYBACKCOMMANDS_H
#define PLAYBACKCOMMANDS_H

#include "common.h"
#include "ConfigVars.h"

namespace foo_mpdsrv
{
	/**
	 * Class for song selection
	 * @author Cookiemon
	 * @todo This was a bad idea
	 */
	class SongSelector
	{
	protected:
		// playlist position of a song
		t_size _songNum;
	public:
		/**
		 * Initializes with no selected song
		 * @author Cookiemon
		 */
		SongSelector();
		/**
		 * Returns song playlist position
		 * @author Cookiemon
		 */
		t_size GetSongNum();
	};
	
	/**
	 * Class selecting song by a playlist position number
	 * @author Cookiemon
	 */
	class SongNumSelector : public SongSelector
	{
	public:
		/**
		 * Creates object by a playist position number
		 * @author Cookiemon
		 */
		explicit SongNumSelector(t_size songNum);
		/**
		 * Creates object by a playist position number string
		 * @author Cookiemon
		 */
		explicit SongNumSelector(const char* songNum);
		/**
		 * Creates object by a playist position number string
		 * @author Cookiemon
		 */
		explicit SongNumSelector(const std::string& songNum);
	};
	
	/**
	 * Class selecting song by a song id
	 * @author Cookiemon
	 */
	class SongIdSelector : public SongSelector
	{
	private:
		/**
		 * Handles id search
		 * @author Cookiemon
		 */
		class HandleIdSearch : public playlist_manager::enum_items_callback
		{
		private:
			// Id to search for
			idtype _searchedItem;
			// Position of the item with searched id
			t_size _pos;
		public:
			/**
			 * Initializes the searcher with a target to search
			 * @author Cookiemon
			 */
			explicit HandleIdSearch(idtype searchedItem);
			/**
			 * Callback that checks the current item
			 * @author Cookiemon
			 * @param p_index position of the current item
			 * @param p_location handle to the current item
			 * @param b_selected unused
			 */
			virtual bool on_item(t_size p_index, const metadb_handle_ptr & p_location, bool b_selected);
			/**
			 * Returns the position of the found item
			 * @author Cookiemon
			 * @return Position of the item (numeric_limits::max() if not found)
			 */
			t_size GetResult();
		};
	public:
		/**
		 * Initializes the object
		 * @author Cookiemon
		 * @param id Id of the searched song
		 * @param playlist Playlist to search on
		 */
		SongIdSelector(idtype id, t_size playlist);
	private:
		/**
		 * Initializes the object
		 * @author Cookiemon
		 * @param id Id of the searched song
		 * @param playlist Playlist to search on
		 */
		void InitId(idtype id, t_size playlist);
	};
	
	/**
	 * Class for playlist selection
	 * @author Cookiemon
	 * @todo This was a bad idea
	 */
	class PlaylistSelector
	{
	protected:
		// Id of the playlist
		t_size _plNum;
	public:
		/**
		 * Initializes the playlist with an invalid playlist
		 * @author Cookiemon
		 */
		PlaylistSelector();
		/**
		 * Returns the selected playlist
		 * @author Cookiemon
		 * @return Playlist id
		 */
		t_size GetPlaylistNum();
	};
	
	/**
	 * Class for playlist selecting the currently playing playlist
	 * @author Cookiemon
	 */
	class DefaultPlaylistSelector : public PlaylistSelector
	{
	public:
		/**
		 * Selects the currently playing playlist
		 * @author Cookiemon
		 */
		DefaultPlaylistSelector();
	};
	
	/**
	 * Class for playlist selection by name
	 * @author Cookiemon
	 */
	class PlaylistNameSelector : public PlaylistSelector
	{
	public:
		/**
		 * Selects the playlist by name
		 * @author Cookiemon
		 * @param name Name of the playlist to select
		 */
		explicit PlaylistNameSelector(const char* name);
		/**
		 * Selects the playlist by name
		 * @author Cookiemon
		 * @param name Name of the playlist to select
		 */
		explicit PlaylistNameSelector(const std::string& name);
		/**
		 * Selects the playlist by name
		 * @author Cookiemon
		 * @param name Name of the playlist to select
		 */
		explicit PlaylistNameSelector(const pfc::string_base& name);
		/**
		 * Selects the playlist by name
		 * @author Cookiemon
		 * @param name Name of the playlist to select
		 */
		void Select(const pfc::string_base& name);
	};
	
	/**
	 * Plays item according to the selected playlist and song
	 * @author Cookiemon
	 * @todo Better idea for selectors
	 * @param name Name of the playlist to select
	 */
	void PlayItem(PlaylistSelector pl, SongSelector song);
}

#endif