#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include "common.h"
#include "resource.h"
#include <atlddx.h>
#include <stdint.h>

namespace foo_mpdsrv
{
	/**
	 * Configuration dialog
	 * @author Cookiemon
	 * @todo <window_service_impl>
	 */
	class ConfigDialogInstance : public preferences_page_instance,
		public CDialogImpl <ConfigDialogInstance>
	{
		BEGIN_MSG_MAP(ConfigDialogInstance)
			MSG_WM_INITDIALOG(OnInit)
			COMMAND_HANDLER_EX(IDC_PORT, EN_CHANGE, OnChangedItem)
			COMMAND_HANDLER_EX(IDC_NETWORKINTERFACE, EN_CHANGE, OnChangedItem)
			COMMAND_HANDLER_EX(IDC_LIBRARYPATH, EN_CHANGE, OnChangedItem)
			COMMAND_HANDLER_EX(IDC_AUTOSTART, BN_CLICKED, OnChangedItem)
		END_MSG_MAP()
	private:
		const preferences_page_callback::ptr _callback;
	public:
		enum { IDD = IDD_SERVERPREFERENCESPAGE };

	public:
		/**
		 * Instantiate config dialog
		 * @author Cookiemon
		 * @param callback Callback that is used for status updates (e.g. apply button available)
		 */
		explicit ConfigDialogInstance(preferences_page_callback::ptr callback);

		/**
		 * Initialize dialog (Shows saved values)
		 * @author Cookiemon
		 * @param wnd unused
		 * @param lParam unused
		 */
		BOOL OnInit(CWindow wnd, LPARAM lParam);
		/**
		 * Updates status
		 * @author Cookiemon
		 * @param wNotifyCode unused
		 * @param ctrl unused
		 * @param HWND unused
		 */
		void OnChangedItem(UINT wNotifyCode, int ctrl, HWND hWnd);
		/**
		 * Checks if values in dialog have been changed
		 * i.e. are not equal to stored values
		 * @author Cookiemon
		 * @return true iff stored values differ from dialog values
		 */
		bool HasChanged();
		
		/**
		 * Returns state of the dialog
		 * @author Cookiemon
		 * @return bitwise or of following flags if they apply: resettable, changed
		 */
		t_uint32 get_state();
		/**
		 * Stores the dialog values in the config file
		 * @author Cookiemon
		 */
		virtual void apply();
		/**
		 * Resets all values to default
		 * @author Cookiemon
		 */
		virtual void reset();
	private:
		/**
		 * Loads stored values into the dialog fields
		 * @author Cookiemon
		 */
		void ShowSavedValues();
	};

	/**
	 * Preferences page in config branch
	 * @author Cookiemon
	 */
	class ConfigDialog : public preferences_page_impl<ConfigDialogInstance>
	{
	private:
		static const GUID ConfigDialogGUID;
		static const char* ConfigDialogName;

	public:
		/**
		 * Returns the name of the dialog branch item
		 * @author Cookiemon
		 * @return Name of the dialog
		 */
		virtual const char* get_name();
		/**
		 * Returns GUID of the dialog branch item
		 * @author Cookiemon
		 * @return GUID
		 */
		virtual GUID get_guid();
		/**
		 * Returns GUID of the parent dialog branch item
		 * @author Cookiemon
		 * @return Parent GUID
		 */
		virtual GUID get_parent_guid();
	};
}

#endif