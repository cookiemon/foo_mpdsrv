#ifndef LIBRARYCONSISTENCYCHECK_H
#define LIBRARYCONSISTENCYCHECK_H

#include <unordered_set>
#include "common.h"
#include "ConfigVars.h"

namespace foo_mpdsrv
{
	/**
	 * Checks the mpd ids for consistency
	 * @author Cookiemon
	 */
	class LibraryConsistencyCheck : public library_manager::enum_callback
	{
	private:
		std::unordered_set<idtype> set;

	public:
		/**
		 * Callback for metadb
		 * Checks if id has already been used and
		 * assigns a new one if necessary
		 * @author Cookiemon
		 * @param p_item Item that is checked
		 * @return true
		 */
		virtual bool on_item(const metadb_handle_ptr& p_item);
		/**
		 * Generates a new id and increments the max counter
		 * @attention IS NOT SIDEEFFECT FREE BECAUSE OF COUNTER INCREMENT
		 * @author Cookiemon
		 * @return New (hopefully but not guaranteed unique) id
		 */
		static idtype GenerateNewId();
		/**
		 * Returns the id of the given metadb item
		 * assigns a new id if necessary
		 * @author Cookiemon
		 * @param p_item Item for which the id is returned
		 * @return Id of p_item
		 */
		static idtype GetId(metadb_handle_ptr p_item);
		/**
		 * Returns the id of the given metadb item
		 * assigns a new id if necessary.
		 * Can be used if file_info for p_item has already
		 * been opened
		 * @author Cookiemon
		 * @param p_item Item for which the id is returned
		 * @param fi file_info of p_item
		 * @return Id of p_item
		 */
		static idtype GetId(metadb_handle_ptr p_item, const file_info& fi);
		/**
		 * Sets a new id for a metadb item
		 * @author Cookiemon
		 * @param newId New id of p_item
		 * @param p_item Item for which the id is set
		 */
		static void SetId(idtype newId, metadb_handle_ptr p_item);
		/**
		 * Sets a new id for a metadb item.
		 * Can be used if file_info for p_item has already
		 * been opened. (Copies const file_info to a new one)
		 * @author Cookiemon
		 * @param newId New id of p_item
		 * @param fi file_info of p_item
		 * @param p_item Item for which the id is set
		 */
		static void SetId(metadb_handle_ptr p_item, const file_info& fi, idtype newId);
		/**
		 * Sets a new id for a metadb item.
		 * Can be used if file_info for p_item has already
		 * been opened. (Copies const file_info to a new one)
		 * @author Cookiemon
		 * @param newId New id of p_item
		 * @param fi file_info of p_item
		 * @param p_item Item for which the id is set
		 */
		static void SetId(metadb_handle_ptr p_item, file_info& fi, idtype newId);
	};
	
	/**
	 * Worker thread that checks the library consistency
	 * and displays consistency check progress
	 * @author Cookiemon
	 */
	class LibraryConsistencyWorker : public threaded_process_callback
	{
	private:
		// {98B45421-E5D9-419B-BEBA-24F381C73484}
		static const GUID class_guid;

		LibraryConsistencyCheck _conCheck;
		pfc::list_t<metadb_handle_ptr> libItems;
		int _ctr;

	public:
		/**
		 * 0-initializes everything
		 */
		LibraryConsistencyWorker();
		/**
		 * Initializes working environment
		 * retrieves library items
		 * @author Cookiemon
		 * @param hWnd unused
		 */
		virtual void on_init(HWND hWnd);
		/**
		 * Updates progress bar and lets all items
		 * be checked.
		 * @author Cookiemon
		 * @param p_status Status bar to display status on
		 * @param p_abort Abort callback
		 */
		virtual void run(threaded_process_status& p_status, abort_callback& p_abort);
		/**
		 * Add one reference to service counter
		 * @author Cookiemon
		 */
		virtual int service_add_ref() throw();
		/**
		 * Remove one reference to service counter
		 * @author Cookiemon
		 */
		virtual int service_release() throw();
	};
}

#endif