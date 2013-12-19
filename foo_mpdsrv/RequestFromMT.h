#ifndef REQUESTFROMMT_H
#define REQUESTFROMMT_H

#include "common.h"
#include <functional>

namespace foo_mpdsrv
{
	/**
	 * Requests information from the mainthread
	 * All functions block until the callback of the
	 * main thread finishes
	 * @todo Seperate information requesting function
	 * so that on new requesters not all tu must be
	 * recompiled
	 * @author Cookiemon
	 */
	class RequestFromMT
	{
	private:
		// Event that is triggered when a callback finishes
		win32_event _event;

	public:
		/**
		 * Initializes waiting event
		 * @author Cookiemon
		 */
		RequestFromMT()
		{
			_event.create(false, false);
		}
		/**
		 * Requests handles to all items in the media library
		 * @author Cookiemon
		 * @param out List of handles to media library
		 */
		void RequestLibraryItems(pfc::list_base_t<metadb_handle_ptr>& out)
		{
			DoCallback([&]()
			{
				static_api_ptr_t<library_manager> inst;
				inst->get_all_items(out);
			});
		}
		/**
		 * Requests relative path of a metadb item
		 * @author Cookiemon
		 * @param item Item for which relative path is requested
		 * @param out relative path for item
		 * @returns false If item is not in media library
		 */
		bool RequestRelativePath(const metadb_handle_ptr item, pfc::string_base& out)
		{
			bool retVal;
			DoCallback([&]()
			{
				static_api_ptr_t<library_manager> inst;
				retVal = inst->get_relative_path(item, out);
			});
			return retVal;
		}
		/**
		 * Requests number of playlists
		 * @author Cookiemon
		 * @param plCount Number of playlists
		 */
		void RequestPlaylistCount(t_size& plCount)
		{
			DoCallback([&]()
			{
				static_api_ptr_t<playlist_manager> inst;
				plCount = inst->get_playlist_count();
			});
		}
		/**
		 * Requests name of a playlist
		 * @author Cookiemon
		 * @param plId Id of playlist for which name is requested
		 * @param name Name of the playlist
		 */
		void RequestPlaylistName(t_size plId, pfc::string_base& name)
		{
			DoCallback([&]()
			{
				static_api_ptr_t<playlist_manager> inst;
				inst->playlist_get_name(plId, name);
			});
		}
		/**
		 * Requests handles to all items of a playlist
		 * @author Cookiemon
		 * @param plId Id of playlist for which items are requested
		 * @param out List of handles to playlist items
		 */
		void RequestPlaylistItems(t_size plId, pfc::list_base_t<metadb_handle_ptr>& out)
		{
			DoCallback([&]()
			{
				static_api_ptr_t<playlist_manager> inst;
				inst->playlist_get_all_items(plId, out);
			});
		}
		/**
		 * Requests number of songs on a playlist
		 * @author Cookiemon
		 * @param plId Id of playlist for which item count is requested
		 * @param out Number of songs on the playlist
		 */
		void RequestPlaylistItemCount(t_size plId, t_size& out)
		{
			DoCallback([&]()
			{
				static_api_ptr_t<playlist_manager> inst;
				out = inst->playlist_get_item_count(plId);
			});
		}
		/**
		 * Requests the id of the active playlist (the one that's
		 * selected in the foobar UI)
		 * @author Cookiemon
		 * @param plId Id of the active playlist
		 */
		void RequestActivePlaylist(t_size& plId)
		{
			DoCallback([&]()
			{
				static_api_ptr_t<playlist_manager> inst;
				plId = inst->get_active_playlist();
			});
		}
		/**
		 * Requests playlist from which playing items are
		 * taken (fallback to active playlist if playback
		 * is stopped)
		 * @author Cookiemon
		 */
		void RequestPlayingPlaylist(t_size& pl)
		{
			DoCallback([&]()
				{
				static_api_ptr_t<playlist_manager> inst;
				pl = inst->get_playing_playlist();
				if(pl == std::numeric_limits<t_size>::max())
					pl = inst->get_active_playlist();
			});
		}
		/**
		 * Requests a bunch of playback information
		 * @author Cookiemon
		 * @param isPlaying true iff playback is not stopped
		 * @param isPaused true iff playback is paused
		 * @param itemResult true iff playingItem could be retrieved
		 * @param playingItem currently playing item
		 * @param position Position in the currently playing track in seconds
		 * @param length Length of the currently playing track in seconds
		 * @param volume Current volume on a logarithmic scale
		 */
		void RequestPlaybackInfo(bool& isPlaying,
		                         bool& isPaused,
		                         bool& itemResult,
		                         metadb_handle_ptr& playingItem,
		                         double& position,
		                         double& length,
		                         float& volume)
		{
			DoCallback([&](){
				static_api_ptr_t<playback_control> playback;
				itemResult = playback->get_now_playing(playingItem);
				isPlaying = playback->is_playing();
				isPaused = playback->is_paused();
				volume = playback->get_volume();
				position = playback->playback_get_position();
				length = playback->playback_get_length();
			});
		}
		/**
		 * Requests currently playing item
		 * @author Cookiemon
		 * @param playing handle to the currently playing item
		 * @return true iff an item is currently played
		 */
		bool RequestNowPlaying(metadb_handle_ptr& playing)
		{
			bool isPlaying;
			DoCallback([&](){
				static_api_ptr_t<playback_control> playback;
				isPlaying = playback->get_now_playing(playing);
			});
			return isPlaying;
		}
		/**
		 * Requests volume from the mainthread
		 * @author Cookiemon
		 * @param volume Current volume on a logarithmic scale
		 */
		void RequestVolume(float& volume)
		{
			DoCallback([&](){
				static_api_ptr_t<playback_control> playback;
				volume = playback->get_volume();
			});
		}
		/**
		 * Requests location of the currently playing item
		 * @author Cookiemon
		 * @param plId Id of the currently playing playlist
		 * @param item Position of the currently playing item
		 * @return true iff the currently playing item is on the playing playlist
		 */
		bool RequestPlayingItemLocation(t_size& plId, t_size& item)
		{
			bool retVal;
			DoCallback([&](){
				static_api_ptr_t<playlist_manager> plman;
				retVal = plman->get_playing_item_location(&plId, &item);
			});
			return retVal;
		}
		/**
		 * Does a callback in the main thread
		 * @author Cookiemon
		 * @tparam T Type of the function/functor
		 * @param func Callback function which executes in the main thread
		 */
		template<typename T>
		void DoCallback(T& func)
		{
#ifdef FOO_MPDSRV_THREADED
			service_ptr_t<InternalCallback<T> > caller(new InternalCallback<T>(_event.get(), func));
			static_api_ptr_t<main_thread_callback_manager> mtfac;
			mtfac->add_callback(caller);
			_event.wait_for(-1);
#else
			func();
#endif
		}
		/**
		 * Does a foobar API call in the main thread
		 * @author Cookiemon
		 * @tparam T Type of the requested API
		 * @param func API-function that should be called
		 */
		template<class T>
		void DoApiCall(void (T::*func)())
		{
			DoCallback([&]() {
				static_api_ptr_t<T> api;
				((api.get_ptr())->*func)();
			});
		}
		/**
		 * Does a foobar API call in the main thread
		 * @author Cookiemon
		 * @tparam T Type of the requested API
		 * @tparam U Type of the first argument
		 * @param func API-function that should be called
		 * @param arg1 First argument that should be passed to func
		 */
		template<class T, typename U>
		void DoApiCall(void (T::*func)(U), U arg1)
		{
			DoCallback([&]() {
				static_api_ptr_t<T> api;
				((api.get_ptr())->*func)(arg1);
			});
		}
		/**
		 * Does a foobar API call in the main thread
		 * @author Cookiemon
		 * @tparam T Type of the requested API
		 * @tparam U Type of the first argument
		 * @tparam V Type of the second argument
		 * @param func API-function that should be called
		 * @param arg1 First argument that should be passed to func
		 * @param arg2 Second argument that should be passed to func
		 */
		template<class T, typename U, typename V>
		void DoApiCall(void (T::*func)(U, V), U arg1, V arg2)
		{
			DoCallback([&]() {
				static_api_ptr_t<T> api;
				((api.get_ptr())->*func)(arg1, arg2);
			});
		}

	private:
		/**
		 * Thread callback structure that gets callbacked in
		 * the mainthread and executes a given function and
		 * signals the event when it is finished
		 * @author Cookiemon
		 * @tparam T Type of the function to call
		 */
		template<typename T>
		class InternalCallback : public main_thread_callback
		{
		private:
			HANDLE _event;
			T& _call;

		public:
			/**
			 * Binds the event and function to this object
			 * @author Cookiemon
			 * @param ev Event that is signaled on finishing the function
			 * @param call Function to call in main thread
			 */
			InternalCallback(HANDLE ev, T& call) : _event(ev), _call(call)
			{
				/* Do nothing */
			}
			
			/**
			 * Calls the function and signals the event
			 * @author Cookiemon
			 */
			virtual void callback_run()
			{
				_call();
				SetEvent(_event);
			}

			virtual int service_release() throw() { return 1; }
			virtual int service_add_ref() throw() { return 1; }
		};
	};
}

#endif