#include "PlaybackCommandHandler.h"
#include "MessageSender.h"
#include "LibraryConsistencyCheck.h"
#include "PlaybackCommands.h"
#include "RequestFromMT.h"
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

	
	template<class T>
	void SimpleApiMTCall(void (T::*func)())
	{
		RequestFromMT req;
		req.DoCallback([&](){
			static_api_ptr_t<T> api;
			((api.get_ptr())->*func)();
		});
	}
	template<class T, typename U>
	void SimpleApiMTCall(void (T::*func)(U), U firstArg)
	{
		RequestFromMT req;
		req.DoCallback([&](){
			static_api_ptr_t<T> api;
			((api.get_ptr())->*func)(firstArg);
		});
	}
	template<class T, typename U, typename V>
	void SimpleApiMTCall(void (T::*func)(U, V), U firstArg, V secondArg)
	{
		RequestFromMT req;
		req.DoCallback([&](){
			static_api_ptr_t<T> api;
			((api.get_ptr())->*func)(firstArg, secondArg);
		});
	}

	void HandlePlay(MessageSender&, std::vector<std::string>& args)
	{
		PlaylistSelector pl = DefaultPlaylistSelector();
		SongSelector song;
		if(args.size() >= 2)
		{
			song = SongNumSelector(args[1]);
		}
		PlayItem(pl, song);
	}

	void HandlePlayId(MessageSender&, std::vector<std::string>& args)
	{
		PlaylistSelector pl = DefaultPlaylistSelector();
		idtype songId;
		ConvertTo(args[1], songId);
		SongSelector song;
		if(args.size() >= 2)
		{
			RequestFromMT reqmt;
			reqmt.DoCallback([&](){
				song = SongIdSelector(songId, pl.GetPlaylistNum());
			});
		}
		PlayItem(pl, song);
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
		SimpleApiMTCall(&playback_control::stop);
	}

	void HandleNext(MessageSender&, std::vector<std::string>&)
	{
		SimpleApiMTCall(&playback_control::start, playback_control::track_command_next, false);
	}

	void HandlePrevious(MessageSender&, std::vector<std::string>&)
	{
		SimpleApiMTCall(&playback_control::start, playback_control::track_command_prev, false);
	}

	void HandleStatus(MessageSender& caller, std::vector<std::string>&)
	{
		caller.SendStatus();
	}

	void HandleVolume(MessageSender& caller, std::vector<std::string>& args)
	{
		static_api_ptr_t<playback_control> man;
		if(args.size() < 2)
			throw CommandException(ACK_ERROR_ARG, "Command needs argument");
		float vol;
		ConvertTo(args[1], vol);
		if(vol < 0 || 100 < vol)
			throw CommandException(ACK_ERROR_ARG, "Argument out of range");
		vol -= 100;
		SimpleApiMTCall(&playback_control::set_volume, vol);
	}
}