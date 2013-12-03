// Known bugs:
//  * Volume on a logarithmic scale

#include "MPDMessageHandler.h"
#include "common.h"
#include "PlaybackCommandHandler.h"
#include "PlaylistCommandHandler.h"
#include "GeneralCommandHandler.h"
#include "DataRequestHandler.h"
#include <algorithm>
#include <functional>
#include <fstream>
#include <unordered_map>

namespace foo_mpdsrv
{
	MPDMessageHandler::MPDMessageHandler(MPDMessageHandler&& right) : _sender(std::move(right._sender)),
		_actions(std::move(right._actions))
	{
		_accumulateList = right._accumulateList;
		_list_OK = right._list_OK;
	}

	MPDMessageHandler::MPDMessageHandler(SOCKET connection) : _accumulateList(false), _list_OK(false), _sender(connection)
	{
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
#ifdef FOO_MPDSRV_THREADED
		ExitThread();
		WaitTillThreadDone();
#endif
	}
	
	void MPDMessageHandler::PushBuffer(const char* buf, size_t numBytes)
	{
		_buffer.write(buf, numBytes);
#ifdef FOO_MPDSRV_THREADED
		Wake();
#else
		HandleBuffer();
#endif
	}

	void MPDMessageHandler::HandleBuffer()
	{
		Logger log(Logger::FINEST);
		log.Log("==>Handling Buffer\n");
		std::string nextCommand;
		std::getline(_buffer, nextCommand);
		log.Log("==>Next Command: ");
		log.Log(nextCommand);
		if(!_lastIncomplete.empty())
		{
			nextCommand = _lastIncomplete + nextCommand;
			_lastIncomplete.clear();
		}
		while(_buffer.good())
		{
			{
				Logger log(Logger::DBG);
				log.Log("I: ");
				log.Log(nextCommand);
			}
			if(!_accumulateList)
			{
				if(strstartswithi(nextCommand, "command_list_begin"))
				{
					_accumulateList = true;
					_list_OK = false;
				}
				else if(strstartswithi(nextCommand, "command_list_ok_begin"))
				{
					_accumulateList = true;
					_list_OK = true;
				}
				else
				{
					try
					{
						size_t lastSpace = nextCommand.find_last_not_of(" \r\n\t");
						if(lastSpace != std::string::npos)
							nextCommand.erase(lastSpace+1);
						ExecuteCommand(nextCommand);
						_sender.SendOk();
					}
					catch(const CommandException& e)
					{
						_sender.SendError(0, nextCommand, e);
					}
				}
			}
			else
			{
				if(!strstartswithi(nextCommand, "command_list_end"))
					_commandQueue.push_back(nextCommand);
				else
				{
					ExecuteCommandQueue(_list_OK);
				}
			}
			std::getline(_buffer, nextCommand);
		}
#ifdef _DEBUG
			{
				Logger log(Logger::DBG);
				log.Log("Bufferstate: ");
				log.Log(_buffer.good()?"Good":"");
				log.Log(_buffer.bad()?"Bad":"");
				log.Log(_buffer.fail()?"Fail":"");
				log.Log(_buffer.eof()?"EOF":"");
			}
#endif
		_lastIncomplete = nextCommand;
		_buffer.str("");
		_buffer.seekg(nextCommand.size());
		_buffer.clear();
	}

	bool MPDMessageHandler::WakeProc(abort_callback &abort)
	{
		Logger log(Logger::FINEST);
		log.Log("==>MPDMessageHandler got pushed buffer\n");
		try
		{
			HandleBuffer();
		}
		catch(const std::exception& e)
		{
			Logger log(Logger::SEVERE);
			log.Log("Caught exception: ");
			log.Log(e.what());
		}
		return true;
	}

	void MPDMessageHandler::ExecuteCommand(std::string message)
	{
		std::vector<std::string> cmd = SplitCommand(message);
		if(cmd.empty())
			return;
		std::transform(cmd[0].begin(), cmd[0].end(), cmd[0].begin(), tolower);

		auto action = _actions.find(cmd[0]);
		if(action != _actions.end())
			action->second(_sender, cmd);
		else
		{
#ifdef _DEBUG
			popup_message::g_show(message.c_str(), "Could not process");
			{
				Logger log(Logger::WARN);
				log.Log("Command not found: ");
				log.Log(message);
			}
#endif
		}
	}
	
	void MPDMessageHandler::ExecuteCommandQueue(bool respondAfterEvery)
	{
		size_t i = 0;
		try
		{
			if(respondAfterEvery)
			{
				for(i = 0; i < _commandQueue.size(); ++i)
				{
					ExecuteCommand(_commandQueue[i]);
					_sender.SendListOk();
				}
				_sender.SendOk();
			}
			else
			{
				for(size_t i = 0; i < _commandQueue.size(); ++i)
				{
					ExecuteCommand(_commandQueue[i]);
				}
				_sender.SendOk();
			}
		}
		catch(const CommandException& e)
		{
			_sender.SendError(i, _commandQueue[i], e);
		}
		_commandQueue.clear();
		_accumulateList = false;
	}

	std::vector<std::string> MPDMessageHandler::SplitCommand(const std::string& cmd)
	{
		std::vector<std::string> ret;
		size_t start = 0;
		size_t length = cmd.length();
		while(start < length && start != std::string::npos)
		{
			size_t end;
			if(cmd[start] == '"')
				end = cmd.find('\"', ++start);
			else
				end = cmd.find_first_of(" \t\r\n", start + 1);

			ret.push_back(cmd.substr(start, end - start));
				
			if(end != std::string::npos)
				start += end + 1;
			else
				start = std::string::npos;
			start = cmd.find_first_not_of(" \t\r\n", start);
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