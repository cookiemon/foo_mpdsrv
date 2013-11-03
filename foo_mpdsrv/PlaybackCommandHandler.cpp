#include "PlaybackCommandHandler.h"
#include "MessageSender.h"
#include "LibraryConsistencyCheck.h"
#include "common.h"
#include <string>

namespace foo_mpdsrv
{
	// Local functions
	void ChangePlaybackState(PlayState::PlayState newState);

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

	class HandleItemsPlay : public playlist_manager::enum_items_callback
	{
	public:
		idtype searchedItem;
		virtual bool on_item(t_size p_index,const metadb_handle_ptr & p_location,bool b_selected)
		{
			if(LibraryConsistencyCheck::GetId(p_location) == searchedItem)
			{
				static_api_ptr_t<playlist_manager> api;
				api->activeplaylist_execute_default_action(p_index);
				return false;
			}
			return true;
		}
	};

	void HandlePlay(MessageSender&, std::vector<std::string>& args)
	{
		long itemnr = -1;
		if(args.size() >= 2)
		{
			char* end;
			itemnr = strtol(args[1].c_str(), &end, 10);
			if(*end != '\0')
				throw CommandException(ACK_ERROR_ARG, "argument 1 not a number");
			static_api_ptr_t<playlist_manager> playlist;
			playlist->activeplaylist_execute_default_action(itemnr);
			HandleItemsPlay handler;
			handler.searchedItem = itemnr;
			playlist->activeplaylist_enum_items(handler, bit_array_true());
		}
		if(itemnr == -1)
		{
			static_api_ptr_t<playback_control> control;
			if(control->is_paused())
				control->pause(false);
			else
				control->start();
		}
	}

	void HandlePlayId(MessageSender&, std::vector<std::string>& args)
	{
		long id = 0;
		if(args.size() >= 2)
		{
			char* end;
			id = strtol(args[1].c_str(), &end, 10);
			if(*end != '\0')
				throw CommandException(ACK_ERROR_ARG, "argument 1 not a number");
			static_api_ptr_t<playlist_manager> playlist;
			HandleItemsPlay handler;
			handler.searchedItem = id;
			playlist->activeplaylist_enum_items(handler, bit_array_true());
		}
		if(id == 0)
		{
			static_api_ptr_t<playback_control> control;
			if(control->is_paused())
				control->pause(false);
			else
				control->start();
		}
	}

	void HandlePause(MessageSender&, std::vector<std::string>& args)
	{
		if(args.size() < 2)
			SimpleApiCall(&playback_control::toggle_pause);
		else
			SimpleApiCall(&playback_control::pause, (args[1] != "0"));
	}

	void ChangePlaybackState(PlayState::PlayState newState)
	{
		static_api_ptr_t<playback_control> control;
		switch(newState)
		{
		case PlayState::Play:
			if(control->is_paused())
				control->pause(false);
			else
				control->start();
			break;
		case PlayState::Pause:
			control->toggle_pause();
			break;
		case PlayState::Stop:
			control->stop();
			break;
		}
	}

	void HandleStop(MessageSender&, std::vector<std::string>&)
	{
		SimpleApiCall(&playback_control::stop);
	}

	void HandleNext(MessageSender&, std::vector<std::string>&)
	{
		SimpleApiCall(&playback_control::start, playback_control::track_command_next, false);
	}

	void HandlePrevious(MessageSender&, std::vector<std::string>&)
	{
		SimpleApiCall(&playback_control::start, playback_control::track_command_prev, false);
	}

	void HandleStatus(MessageSender& caller, std::vector<std::string>&)
	{
		caller.SendStatus();
	}
}