#ifndef REQUESTFROMMT_H
#define REQUESTFROMMT_H

#include "common.h"
#include <functional>

namespace foo_mpdsrv
{
	class RequestFromMT
	{
	private:
		win32_event _event;

	public:
		RequestFromMT()
		{
			_event.create(false, false);
		}

		void RequestPlaylistItems(t_size pl, pfc::list_base_t<metadb_handle_ptr>& out)
		{
			DoCallback([&]()
			{
				static_api_ptr_t<playlist_manager> inst;
				inst->playlist_get_all_items(pl, out);
			});
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
			});
		}
	private:
		template<typename T>
		void DoCallback(T& func)
		{
			service_ptr_t<InternalCallback<T> > caller(new InternalCallback<T>(_event.get(), func));
			static_api_ptr_t<main_thread_callback_manager> mtfac;
			mtfac->add_callback(caller);
			_event.wait_for(-1);
		}

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