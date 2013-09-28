#include "DataRequestHandler.h"
#include "common.h"

namespace foo_mpdsrv
{
	// Local functions and classes
	class SortByFolder;
	void FilterListByPath(pfc::list_t<metadb_handle_ptr>& out, const pfc::string8& start);
	void NormalizePath(pfc::string8& path);

	
	void HandleStats(MessageSender& caller, std::vector<std::string>&)
	{
		caller.SendStats();
	}

	void HandleOutputs(MessageSender& caller, std::vector<std::string>&)
	{
		caller.SendOutputs();
	}

	void HandleCurrentsong(MessageSender& caller, std::vector<std::string>&)
	{
		static_api_ptr_t<playback_control> playback;

		metadb_handle_ptr playing;
		if(playback->get_now_playing(playing))
		{
			caller.SendSongMetadata(playing);
		}
	}

	class SortByFolder : public pfc::list_base_t<metadb_handle_ptr>::sort_callback
	{
		static_api_ptr_t<library_manager> man;
		virtual int compare(const metadb_handle_ptr& p_item1,const metadb_handle_ptr& p_item2)
		{
			pfc::string8 name1;
			pfc::string8 name2;
			man->get_relative_path(p_item1, name1);
			man->get_relative_path(p_item2, name2);
			return strcmp(name1.get_ptr(), name2.get_ptr());
		}
	};

	void HandleLsinfo(MessageSender& caller, std::vector<std::string>& args)
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

		pfc::string8 lastFolder;
		pfc::string8 currentFolder;
		t_size libSize = out.get_count();
		t_size i = 0;

		t_size pathLength = path.length();
		while(i < libSize && lib->get_relative_path(out[i], currentFolder))
		{
			currentFolder.remove_chars(0, pathLength);
			t_size slashIdx = currentFolder.find_first('\\');
			if(slashIdx != ~t_size(0))
			{
				currentFolder.remove_chars(slashIdx, currentFolder.length() - slashIdx - 1);
				currentFolder.add_char('/');
			}
			if(currentFolder != lastFolder)
			{
				caller.SendPath(currentFolder);
				lastFolder = currentFolder;
			}
			++i;
		}

		if(path.is_empty())
		{
			static_api_ptr_t<playlist_manager> plman;
			t_size plcount = plman->get_playlist_count();
			for(t_size i = 0; i < plcount; ++i)
				caller.SendPlaylistPath(i);
		}
	}

	void FilterListByPath(pfc::list_t<metadb_handle_ptr>& out, const pfc::string8& start)
	{
		int num = 0;
		bool intervalStarted = false;
		static_api_ptr_t<library_manager> lib;
		for(t_size i = 0; i < out.get_count(); ++i)
		{
			pfc::string8 filename;
			lib->get_relative_path(out[i], filename);
			if(!strstartswithi(filename.get_ptr(), start.get_ptr()))
			{
				if(intervalStarted)
					num += 1;
				else
				{
					num = 1;
					intervalStarted = true;
				}
			}
			else
			{
				if(intervalStarted)
				{
					i -= num;
					out.remove_from_idx(i, num);
					intervalStarted = false;
				}
			}
		}
		if(intervalStarted)
		{
			out.remove_from_idx(out.get_count() - num, num);
			intervalStarted = false;
		}
	}

	void NormalizePath(pfc::string8& path)
	{
		if(!path.ends_with('/'))
			path.add_char('/');
		if(!(path[0] == '/'))
		{
			pfc::string8 newPath("/");
			newPath.add_string(path);
			path = newPath;
		}
		path.remove_chars(0, 1);
		path.replace_char('/', '\\', 0);
	}
}