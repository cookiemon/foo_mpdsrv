#include "PlaybackCommandHandler.h"
#include "Converter.h"
#include "MessageSender.h"
#include "LibraryConsistencyCheck.h"
#include "PlaybackCommands.h"
#include "RequestFromMT.h"
#include "common.h"
#include "RequestFromMT.h"
#include <string>

namespace foo_mpdsrv
{
	// Local functions
	static void ChangePlaybackState(PlayState::PlayState newState);

	void HandlePlay(MessageSender&, std::vector<std::string>& args)
	{
		TRACK_CALL_TEXT("NetworkInformation::HandlePlay()");
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
		TRACK_CALL_TEXT("NetworkInformation::HandlePlayId()");
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
		TRACK_CALL_TEXT("NetworkInformation::HandlePause()");
		static_api_ptr_t<playback_control> pbc;
		if(args.size() < 2)
			pbc->toggle_pause();
		else
			pbc->pause(args[1] != "0");
	}

	static void ChangePlaybackState(PlayState::PlayState newState)
	{
		TRACK_CALL_TEXT("NetworkInformation::ChangePlaybackState()");
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
		TRACK_CALL_TEXT("NetworkInformation::HandleStop()");
		RequestFromMT mtreq;
		mtreq.DoApiCall(&playback_control::stop);
	}

	void HandleNext(MessageSender&, std::vector<std::string>&)
	{
		TRACK_CALL_TEXT("NetworkInformation::HandleNext()");
		RequestFromMT mtreq;
		mtreq.DoApiCall(&playback_control::start, playback_control::track_command_next, false);
	}

	void HandlePrevious(MessageSender&, std::vector<std::string>&)
	{
		TRACK_CALL_TEXT("NetworkInformation::HandlePrevious()");
		RequestFromMT mtreq;
		mtreq.DoApiCall(&playback_control::start, playback_control::track_command_prev, false);
	}

	void HandleStatus(MessageSender& caller, std::vector<std::string>&)
	{
		TRACK_CALL_TEXT("NetworkInformation::HandleStatus()");
		caller.SendStatus();
	}

	void HandleVolume(MessageSender& caller, std::vector<std::string>& args)
	{
		TRACK_CALL_TEXT("NetworkInformation::HandleVolume()");
		static_api_ptr_t<playback_control> man;
		if(args.size() < 2)
			throw CommandException(ACK_ERROR_ARG, "Command needs argument");
		float vol;
		ConvertTo(args[1], vol);
		if(vol < 0 || 100 < vol)
			throw CommandException(ACK_ERROR_ARG, "Argument out of range");
		vol -= 100;
		RequestFromMT mtreq;
		mtreq.DoApiCall(&playback_control::set_volume, vol);
	}
}