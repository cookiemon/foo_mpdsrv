#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include "common.h"
#include "resource.h"
#include <atlddx.h>
#include <stdint.h>

namespace foo_mpdsrv
{
	class ConfigDialogInstance : public preferences_page_instance,
		public CDialogImpl <ConfigDialogInstance>
	{
	private:
		const preferences_page_callback::ptr _callback;
	public:
		enum { IDD = IDD_SERVERPREFERENCESPAGE };

	public:
		BEGIN_MSG_MAP(ConfigDialogInstance)
			MSG_WM_INITDIALOG(OnInit)
			COMMAND_HANDLER_EX(IDC_PORT, EN_CHANGE, OnChangedItem)
			COMMAND_HANDLER_EX(IDC_NETWORKINTERFACE, EN_CHANGE, OnChangedItem)
			COMMAND_HANDLER_EX(IDC_LIBRARYPATH, EN_CHANGE, OnChangedItem)
			COMMAND_ID_HANDLER(IDC_REFRESH, OnRefresh)
		END_MSG_MAP()

		explicit ConfigDialogInstance(preferences_page_callback::ptr callback);
		BOOL OnInit(CWindow wnd, LPARAM lParam);
		void OnPathChange(UINT foo, int bar, CWindow baz);
		void OnChangedItem(UINT wNotifyCode, int ctrl, HWND hWnd);
		LRESULT OnRefresh(WORD wNotifyCode, WORD wID, HWND hWnd, BOOL& handled);
		bool HasChanged();
		void OnChange();

		t_uint32 get_state();
		void apply();
		void reset();
	private:
		void ShowSavedValues();
	};

	class ConfigDialog : public preferences_page_impl<ConfigDialogInstance>
	{
	private:
		static const GUID ConfigDialogGUID;
		static const char* ConfigDialogName;

	public:
		const char* get_name();
		GUID get_guid();
		GUID get_parent_guid();
	};
}

#endif