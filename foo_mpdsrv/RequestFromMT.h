#ifndef REQUESTFROMMT_H
#define REQUESTFROMMT_H

#include "common.h"
#include <functional>

namespace foo_mpdsrv
{
	template<class T>
	void SimpleApiCall(void (T::*func)())
	{
		static_api_ptr_t<T> api;
		((api.get_ptr())->*func)();
	}
	template<class T, typename U>
	void SimpleApiCall(void (T::*func)(U), U firstArg)
	{
		static_api_ptr_t<T> api;
		((api.get_ptr())->*func)(firstArg);
	}
	template<class T, typename U, typename V>
	void SimpleApiCall(void (T::*func)(U, V), U firstArg, V secondArg)
	{
		static_api_ptr_t<T> api;
		((api.get_ptr())->*func)(firstArg, secondArg);
	}

	class RequestFromMT
	{
	private:
		win32_event _event;

	public:
		RequestFromMT()
		{
			_event.create(false, false);
		}
		void RequestLibraryItems(pfc::list_base_t<metadb_handle_ptr>& out)
		{
			DoCallback([&]()
			{
				static_api_ptr_t<library_manager> inst;
				inst->get_all_items(out);
			});
		}
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
		void RequestPlaylistCount(t_size& plCount)
		{
			DoCallback([&]()
			{
				static_api_ptr_t<playlist_manager> inst;
				plCount = inst->get_playlist_count();
			});
		}
		void RequestPlaylistName(t_size plId, pfc::string_base& name)
		{
			DoCallback([&]()
			{
				static_api_ptr_t<playlist_manager> inst;
				inst->playlist_get_name(plId, name);
			});
		}
		void RequestPlaylistItems(t_size pl, pfc::list_base_t<metadb_handle_ptr>& out)
		{
			DoCallback([&]()
			{
				static_api_ptr_t<playlist_manager> inst;
				inst->playlist_get_all_items(pl, out);
			});
		}
		t_size RequestPlaylistItemCount(t_size playlist)
		{
			t_size retVal;
			DoCallback([&]()
			{
				static_api_ptr_t<playlist_manager> inst;
				retVal = inst->playlist_get_item_count(playlist);
			});
			return retVal;
		}
		void RequestActivePlaylist(t_size& pl)
		{
			DoCallback([&]()
			{
				static_api_ptr_t<playlist_manager> inst;
				pl = inst->get_active_playlist();
			});
		}
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
		void RequestPlaybackInfo(bool& isPlaying,
			bool& isPaused,
			bool& itemResult,
			metadb_handle_ptr& playingItem,
			double& position,
			double& length,
			float& volume
			)
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
		bool RequestNowPlaying(metadb_handle_ptr& playing)
		{
			bool isPlaying;
			DoCallback([&](){
				static_api_ptr_t<playback_control> playback;
				isPlaying = playback->get_now_playing(playing);
			});
			return isPlaying;
		}
		void RequestVolume(float& volume)
		{
			DoCallback([&](){
				static_api_ptr_t<playback_control> playback;
				volume = playback->get_volume();
			});
		}
		bool RequestPlayingItemLocation(t_size* playlist, t_size* item)
		{
			bool retVal;
			DoCallback([&](){
				static_api_ptr_t<playlist_manager> plman;
				retVal = plman->get_playing_item_location(playlist, item);
			});
			return retVal;
		}
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
		template<class T>
		void DoApiCall(void (T::*func)())
		{
			DoCallback([&]() {
				SimpleApiCall<T>(func);
			});
		}
		template<class T, typename U>
		void DoApiCall(void (T::*func)(U), U arg1)
		{
			DoCallback([&]() {
				SimpleApiCall<T, U>(func, arg1);
			});
		}
		template<class T, typename U, typename V>
		void DoApiCall(void (T::*func)(U, V), U arg1, V arg2)
		{
			DoCallback([&]() {
				SimpleApiCall<T, U, V>(func, arg1, arg2);
			});
		}

	private:
		template<typename T>
		struct InternalCallback : public main_thread_callback
		{
			HANDLE _event;
			T _call;

			InternalCallback(HANDLE ev, T& call) : _event(ev), _call(call) { /* Do nothing */ }

			void callback_run()
			{
				_call();
				SetEvent(_event);
			}

			int service_release() throw() { return 1; }
			int service_add_ref() throw() { return 1; }
		};
	};
}

#endif