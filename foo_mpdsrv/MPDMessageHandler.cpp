// Known bugs:
//  * Volume on a logarithmic scale
//  * _buffer must be made thread-safe

#include "MPDMessageHandler.h"
#include "common.h"
#include "StringUtil.h"
#include "PlaybackCommandHandler.h"
#include "PlaylistCommandHandler.h"
#include "GeneralCommandHandler.h"
#include "DataRequestHandler.h"
#include "Timer.h"
#include "MessageTransporter.h"
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

	MPDMessageHandler::MPDMessageHandler(MessageTransporter&& transp) : _sender(std::move(transp))
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
#ifdef FOO_MPDSRV_THREADED
		WriteLockHandle writeLock = _bufferLock.LockWrite();
		_buffer.insert(_buffer.end(), buf, buf + numBytes);
		Wake();
#else
		_buffer.insert(_buffer.end(), buf, buf + numBytes);
		HandleBuffer();
#endif
	}

	void MPDMessageHandler::HandleBuffer()
	{
		TRACK_CALL_TEXT("MPDMessageHandler::HandleBuffer()");
		char newLineChars[] = { '\n', '\r' };
		int blubb = '\x8B';

#ifdef FOO_MPDSRV_THREADED
		// Need lock in multithreaded environment
		WriteLockHandle writeHandle = _bufferLock.LockWrite();
#endif

		BufferType::iterator begin = _buffer.begin();
		BufferType::iterator end = _buffer.end();
		BufferType::iterator newLine;
		while((newLine = std::find_first_of(begin, end, newLineChars, newLineChars + sizeof(newLineChars)))
				!= end)
		{
			std::string nextCommand(begin, newLine);
			Trim(nextCommand);
			begin = newLine + 1;
			if(!nextCommand.empty())
			{
				Logger(Logger::DBG) << "I: " << nextCommand;
				(this->*_activeHandleCommandRoutine)(nextCommand);
			}
		}
		_buffer.erase(_buffer.begin(), begin);
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
		if(!StrStartsWithLC(cmd, "command_list_end"))
			_commandQueue.push_back(cmd);
		else
		{
			ExecuteCommandQueue([this](const std::string& cmd) {
				ExecuteCommand(cmd);
			});
			_sender.SendOk();
			_activeHandleCommandRoutine = &MPDMessageHandler::HandleCommandOnly;
		}
	}
	void MPDMessageHandler::HandleCommandListOkActive(const std::string& cmd)
	{
		if(!StrStartsWithLC(cmd, "command_list_end"))
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
		if(StrStartsWithLC(cmd, "command_list_begin"))
		{
			_activeHandleCommandRoutine = &MPDMessageHandler::HandleCommandListActive;
		}
		else if(StrStartsWithLC(cmd, "command_list_ok_begin"))
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
			{
				start += 1;
				argEnd = std::find(start, end, '"');
			}
			else
			{
				argEnd = std::find_if(start + 1, end, &CharIsSpace);
			}
			ret.push_back(std::string(start, argEnd));
			if(argEnd != cmd.end())
				argEnd += 1;

			start = std::find_if_not(argEnd, end, &CharIsSpace);
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