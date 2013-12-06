#ifndef LIBRARYCONSISTENCYCHECK_H
#define LIBRARYCONSISTENCYCHECK_H

#include <unordered_set>
#include "common.h"
#include "ConfigVars.h"
#include "RequestFromMT.h"
namespace foo_mpdsrv
{
	class LibraryConsistencyCheck : public library_manager::enum_callback
	{
	private:
		std::unordered_set<idtype> set;

	public:
		bool on_item(const metadb_handle_ptr& p_item);
		static idtype GenerateNewId();
		static idtype GetId(metadb_handle_ptr p_item);
		static idtype GetId(metadb_handle_ptr p_item, const file_info& fi);
		static void SetId(idtype newId, metadb_handle_ptr p_item);
		static void SetId(metadb_handle_ptr p_item, const file_info& fi, idtype newId);
		static void SetId(metadb_handle_ptr p_item, file_info& fi, idtype newId);
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
		LibraryConsistencyWorker();
		void on_init(HWND hWnd);
		void run(threaded_process_status& p_status, abort_callback& p_abort);
		int service_add_ref() throw();
		int service_release() throw();
	};
}

#endif