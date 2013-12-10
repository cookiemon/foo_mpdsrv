// Known bugs:
//  * Volume on a logarithmic scale

#include "MPDMessageHandler.h"
#include "common.h"
#include "PlaybackCommandHandler.h"
#include "PlaylistCommandHandler.h"
#include "GeneralCommandHandler.h"
#include "DataRequestHandler.h"
#include "Timer.h"
#include <algorithm>
#include <functional>
#include <fstream>
#include <string>
#include <unordered_map>

namespace foo_mpdsrv
{
	MPDMessageHandler::MPDMessageHandler(MPDMessageHandler&& right) : _sender(std::move(right._sender)),
		_actions(std::move(right._actions)),
		_activeHandleCommandRoutine(&MPDMessageHandler::HandleCommandOnly)
	{
		TRACK_CALL_TEXT("MPDMessageHandler::MPDMessageHandler(&&)");
	}

	MPDMessageHandler::MPDMessageHandler(SOCKET connection) : _sender(connection)
	{
		TRACK_CALL_TEXT("MPDMessageHandler::MPDMessageHandler(socket)");
		_actions.insert(std::make_pair("ping", &HandlePing));
		_actions.insert(std::make_pair("pause", &HandlePause));
		_actions.insert(std::make_pair("play", &HandlePlay));
		_actions.insert(std::make_pair("playid", &HandlePlayId));
		_actions.insert(std::make_pair("stop", &HandleStop));
		_actions.insert(std::make_pair("next", &HandleNext));
		_actions.insert(std::make_pair("previous", &HandlePrevious));
		_actions.insert(std::make_pair("playlistinfo", &HandlePlaylistinfo));
		_actions.insert(std::make_pair("listplaylistinfo", &HandleListplaylistinfo));
		_actions.insert(std::make_pair("lsinfo", &HandleLsinfo));
		_actions.insert(std::make_pair("currentsong", &HandleCurrentsong));
		_actions.insert(std::make_pair("plchanges", &HandlePlchanges));
		_actions.insert(std::make_pair("status", &HandleStatus));
		_actions.insert(std::make_pair("stats", &HandleStats));
		_actions.insert(std::make_pair("outputs", &HandleOutputs));
		_actions.insert(std::make_pair("close", &HandleClose));
		_actions.insert(std::make_pair("volume", &HandleVolume));
		_actions.insert(std::make_pair("setvol", &HandleVolume));
	}

	MPDMessageHandler::~MPDMessageHandler()
	{
		TRACK_CALL_TEXT("MPDMessageHandler::~MPDMessageHandler()");
#ifdef FOO_MPDSRV_THREADED
		ExitThread();
		WaitTillThreadDone();
#endif
	}
	
	void MPDMessageHandler::PushBuffer(const char* buf, size_t numBytes)
	{
		TRACK_CALL_TEXT("MPDMessageHandler::PushBuffer()");
		_buffer.insert(_buffer.end(), buf, buf+numBytes);
#ifdef FOO_MPDSRV_THREADED
		Wake();
#else
		HandleBuffer();
#endif
	}

	void MPDMessageHandler::HandleBuffer()
	{
		TRACK_CALL_TEXT("MPDMessageHandler::HandleBuffer()");
		BufferType::iterator newLine;
		char newLineChars[] = { '\n', '\r' };
		while((newLine = std::find_first_of(_buffer.begin(), _buffer.end(),
											newLineChars, newLineChars + sizeof(newLineChars)))
				!= _buffer.end())
		{
			std::string nextCommand(_buffer.begin(), _buffer.end());
			Trim(nextCommand);
			_buffer.erase(_buffer.begin(), ++newLine);
			if(nextCommand.empty())
				continue;
			{
				Logger(Logger::DBG) << "I: " << nextCommand;
			}
			(this->*_activeHandleCommandRoutine)(nextCommand);
		}
	}

	template<typename T>
	void MPDMessageHandler::ExecuteCommandQueue(T commandLoop)
	{
		TRACK_CALL_TEXT("MPDMessageHandler::ExecuteCommandQueue()");
		size_t i = 0;
		try
		{
			for(i = 0; i < _commandQueue.size(); ++i)
				commandLoop(_commandQueue[i]);
		}
		catch(const CommandException& e)
		{
			_sender.SendError(i, _commandQueue[i], e);
		}
		_commandQueue.clear();
	}

	void MPDMessageHandler::HandleCommandListActive(const std::string& cmd)
	{
		if(!strstartswithi(cmd, "command_list_end"))
			_commandQueue.push_back(cmd);
		else
		{
			ExecuteCommandQueue([this](const std::string& cmd) {
				ExecuteCommand(cmd);
			});
			_activeHandleCommandRoutine = &MPDMessageHandler::HandleCommandOnly;
		}
	}
	void MPDMessageHandler::HandleCommandListOkActive(const std::string& cmd)
	{
		if(!strstartswithi(cmd, "command_list_end"))
			_commandQueue.push_back(cmd);
		else
		{
			ExecuteCommandQueue([this](const std::string& cmd) {
				ExecuteCommand(cmd);
				_sender.SendListOk();
			});
			_activeHandleCommandRoutine = &MPDMessageHandler::HandleCommandOnly;
		}
	}
	void MPDMessageHandler::HandleCommandOnly(const std::string& cmd)
	{
		if(strstartswithi(cmd, "command_list_begin"))
		{
			_activeHandleCommandRoutine = &MPDMessageHandler::HandleCommandListActive;
		}
		else if(strstartswithi(cmd, "command_list_ok_begin"))
		{
			_activeHandleCommandRoutine = &MPDMessageHandler::HandleCommandListOkActive;
		}
		else
		{
			try
			{
				ExecuteCommand(cmd);
				_sender.SendOk();
			}
			catch(const CommandException& e)
			{
				_sender.SendError(0, cmd, e);
			}
		}
	}

	bool MPDMessageHandler::WakeProc(abort_callback &abort)
	{
		TRACK_CALL_TEXT("MPDMessageHandler::WakeProc()");
		Logger(Logger::FINEST) << "==>MPDMessageHandler got pushed buffer";
		try
		{
			HandleBuffer();
		}
		catch(const std::exception& e)
		{
			Logger(Logger::SEVERE) << "Caught exception: " << e;
		}
		return true;
	}

	void MPDMessageHandler::ExecuteCommand(std::string message)
	{
		TRACK_CALL_TEXT("MPDMessageHandler::ExecuteCommand()");
		Timer time;
		std::vector<std::string> cmd = SplitCommand(message);
		std::transform(cmd[0].begin(), cmd[0].end(), cmd[0].begin(), tolower);

		auto action = _actions.find(cmd[0]);
		if(action != _actions.end())
			action->second(_sender, cmd);
		else
		{
			Logger(Logger::DBG) << "Command not found: " << message;
		}
		time.LogDifference("Command " + cmd[0]);
	}

	std::vector<std::string> MPDMessageHandler::SplitCommand(const std::string& cmd)
	{
		assert(!cmd.empty());
		TRACK_CALL_TEXT("MPDMessageHandler::SplitCommand()");
		std::vector<std::string> ret;
		std::string::const_iterator start = cmd.begin();
		std::string::const_iterator end = cmd.end();
		while(start != end)
		{
			std::string::const_iterator argEnd;
			if(*start == '"')
				argEnd = std::find(start + 1, end, '"');
			else
				argEnd = std::find_if(start + 1, end, &isspace);

			ret.push_back(std::string(start, argEnd));
			start = std::find_if_not(argEnd + 1, end, &isspace);
		}
		return ret;
	}

	/*void MPDMessageHandler::SendAllFileinfo(std::vector<std::string>& args)
	{
		if(args.size() < 2)
			args.push_back("/");
		pfc::string8 path = args[1].c_str();
		NormalizePath(path);
		
		static_api_ptr_t<library_manager> lib;
		pfc::list_t<metadb_handle_ptr> out;
		lib->get_all_items(out);
		SortByFolder tmp;
		out.sort(tmp);
		FilterListByPath(out, path);
		
		t_size length = out.get_count();

		for(t_size i = 0; i < length; ++i)
			_sender.SendSongMetadata(out[i]);
	}*/
}