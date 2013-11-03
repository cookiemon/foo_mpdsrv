#include "PlaybackCommandHandler.h"
#include "MessageSender.h"
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

	void HandlePlay(MessageSender&, std::vector<std::string>& args)
	{
		if(args.size() < 2)
		{
			static_api_ptr_t<playback_control> control;
			if(control->is_paused())
				control->pause(false);
			else
				control->start();
		}
		else
		{
			long id;
			char* end;
			id = strtol(args[1].c_str(), &end, 10);
			if(!*end)
				throw CommandException(ACK_ERROR_ARG, "argument 1 not a number");
			static_api_ptr_t<playlist_manager> playlist;
			playlist->activeplaylist_execute_default_action(id);
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