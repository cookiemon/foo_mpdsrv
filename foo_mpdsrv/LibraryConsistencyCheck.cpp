#include "LibraryConsistencyCheck.h"

namespace foo_mpdsrv
{
	const GUID LibraryConsistencyWorker::class_guid = { 0x98b45421, 0xe5d9, 0x419b, { 0xbe, 0xba, 0x24, 0xf3, 0x81, 0xc7, 0x34, 0x84 } };

	bool LibraryConsistencyCheck::on_item(const metadb_handle_ptr& p_item)
	{
		file_info_impl fi;
		p_item->get_info(fi);
		const char* idStr = fi.meta_get(g_IdString, 0);
		bool hasIdTag = idStr != NULL;
		t_size id;
		if(idStr != NULL && pfc::string_is_numeric(idStr))
			id = pfc::atoui64_ex(idStr, pfc_infinite);
		else
			id = GenerateNewId();
		t_size newId = id;
		while(!set.emplace(newId).second)
			newId = GenerateNewId();

		if(!hasIdTag || newId != id)
		{
			Logger(Logger::DBG) << "Duplicate or missing id: " << p_item->get_path();
			SetId(p_item, fi, newId);
		}

		return true;
	}
	idtype LibraryConsistencyCheck::GenerateNewId()
	{
		idtype id = g_MaxId;
		g_MaxId = id + 1;
		return id;
	}
	idtype LibraryConsistencyCheck::GetId(metadb_handle_ptr p_item)
	{
		file_info_impl fi;
		p_item->get_info(fi);
		return GetId(p_item, fi);
	}
	idtype LibraryConsistencyCheck::GetId(metadb_handle_ptr p_item, const file_info& fi)
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
			idtype newId = GenerateNewId();
			SetId(p_item, fi, newId);
			return newId;
		}
	}
	void LibraryConsistencyCheck::SetId(idtype newId, metadb_handle_ptr p_item)
	{
		file_info_impl fi;
		p_item->get_info(fi);
		SetId(p_item, fi, newId);
	}
	void LibraryConsistencyCheck::SetId(metadb_handle_ptr p_item, const file_info& fi, idtype newId)
	{
		file_info_impl nonConstFi;
		nonConstFi.copy(fi);
		SetId(p_item, nonConstFi, newId);
	}
	void LibraryConsistencyCheck::SetId(metadb_handle_ptr p_item, file_info& fi, idtype newId)
	{
		try
		{
			fi.meta_set(g_IdString, pfc::toString(newId).get_ptr());
			service_ptr_t<input_info_writer> instance;
			input_entry::g_open_for_info_write(instance, NULL, p_item->get_path(), abort_callback_dummy());
			instance->set_info(p_item->get_subsong_index(), fi, abort_callback_dummy());
			instance->commit(abort_callback_dummy());
			if(g_MaxId <= newId)
				g_MaxId = newId+1;
		}
		catch(foobar2000_io::exception_io_sharing_violation& e)
		{
			Logger(Logger::DBG) << "Checked file is in use. Trying playback stop.";
			RequestFromMT req;
			req.DoCallback([](){
				static_api_ptr_t<playback_control>()->stop();
			});
				
			fi.meta_set(g_IdString, pfc::toString(newId).get_ptr());
			service_ptr_t<input_info_writer> instance;
			input_entry::g_open_for_info_write(instance, NULL, p_item->get_path(), abort_callback_dummy());
			instance->set_info(p_item->get_subsong_index(), fi, abort_callback_dummy());
			instance->commit(abort_callback_dummy());
			if(g_MaxId <= newId)
				g_MaxId = newId+1;
				
			req.DoCallback([](){
				static_api_ptr_t<playback_control>()->start();
			});
		}
	}
	LibraryConsistencyWorker::LibraryConsistencyWorker() : _ctr(0)
	{
	}

	void LibraryConsistencyWorker::on_init(HWND hWnd)
	{
		static_api_ptr_t<library_manager> foo;
		foo->get_all_items(libItems);
	}
		
	void LibraryConsistencyWorker::run(threaded_process_status& p_status, abort_callback& p_abort)
	{
		TRACK_CALL_TEXT("LibraryConsistencyWorker::run()");
		t_size numLib = libItems.get_count();
		for(t_size i = 0; i < libItems.get_count(); ++i)
		{
			p_status.set_progress(i, numLib);
			p_status.set_item_path(libItems[i]->get_path());
			_conCheck.on_item(libItems[i]);
			p_status.poll_pause();
		}
	}

	int LibraryConsistencyWorker::service_add_ref() throw()
	{
		return _ctr += 1;
	}

	int LibraryConsistencyWorker::service_release() throw()
	{
		return _ctr -= 1;
	}
}