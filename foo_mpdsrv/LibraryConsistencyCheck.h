#ifndef LIBRARYCONSISTENCYCHECK_H
#define LIBRARYCONSISTENCYCHECK_H

#include <unordered_set>
#include "common.h"
#include "ConfigVars.h"

namespace foo_mpdsrv
{
	class LibraryConsistencyCheck : public library_manager::enum_callback
	{
	private:
		std::unordered_set<idtype> set;

	public:
		bool on_item(const metadb_handle_ptr& p_item)
		{
			file_info_impl fi;
			p_item->get_info(fi);
			const char* idStr = fi.meta_get(g_IdString, 0);
			bool hasIdTag = idStr != NULL;
			t_size id;
			if(idStr != NULL && pfc::string_is_numeric(idStr))
				id = pfc::atoui64_ex(idStr, pfc_infinite);
			else
				id = g_MaxId;
			t_size newId = id;
			while(!set.emplace(newId).second)
				newId += 1;

			if(!hasIdTag || newId != id)
			{
				SetId(p_item, fi, newId);
			}

			return true;
		}
		static idtype GetId(metadb_handle_ptr p_item)
		{
			file_info_impl fi;
			p_item->get_info(fi);
			return GetId(p_item, fi);
		}
		static idtype GetId(metadb_handle_ptr p_item, file_info& fi)
		{
			const char* idStr = fi.meta_get(g_IdString, 0);
			char* idStrEnd;
			if(idStr != NULL && pfc::string_is_numeric(idStr))
			{
				idtype id = strtoul(idStr, &idStrEnd, 10);
				return id;
			}
			else
			{
				SetId(p_item, fi, g_MaxId);
				g_MaxId = g_MaxId + 1;
				return g_MaxId - 1;
			}
		}
		static void SetId(idtype newId, metadb_handle_ptr p_item)
		{
			file_info_impl fi;
			p_item->get_info(fi);
			SetId(p_item, fi, newId);
		}
		static void SetId(metadb_handle_ptr p_item, file_info& fi, idtype newId)
		{
			fi.meta_set(g_IdString, pfc::toString(newId).get_ptr());
			service_ptr_t<input_info_writer> instance;
			input_entry::g_open_for_info_write(instance, NULL, p_item->get_path(), abort_callback_dummy());
			instance->set_info(p_item->get_subsong_index(), fi, abort_callback_dummy());
			instance->commit(abort_callback_dummy());
		}
	};
	class LibraryConsistencyWorker : public threaded_process_callback
	{
	private:
		// {98B45421-E5D9-419B-BEBA-24F381C73484}
		static const GUID class_guid;

		LibraryConsistencyCheck _conCheck;
		pfc::list_t<metadb_handle_ptr> libItems;
		int _ctr;

	public:
		LibraryConsistencyWorker() : _ctr(0)
		{
		}

		void on_init(HWND hWnd)
		{
			static_api_ptr_t<library_manager> foo;
			foo->get_all_items(libItems);
		}
		
		void run(threaded_process_status& p_status, abort_callback& p_abort)
		{
			t_size numLib = libItems.get_count();
			for(t_size i = 0; i < libItems.get_count(); ++i)
			{
				p_status.set_progress(i, numLib);
				p_status.set_item_path(libItems[i]->get_path());
				_conCheck.on_item(libItems[i]);
				p_status.poll_pause();
			}
		}

		int service_add_ref() throw()
		{
			return _ctr += 1;
		}

		int service_release() throw()
		{
			return _ctr -= 1;
		}

	};
}

#endif