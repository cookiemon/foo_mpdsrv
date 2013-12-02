#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include "common.h"
#include "resource.h"
#include <atlddx.h>
#include <stdint.h>

namespace foo_mpdsrv
{
	class ConfigDialogInstance : public preferences_page_instance,
		public CDialogImpl <ConfigDialogInstance>,
		public CWinDataExchange<ConfigDialogInstance>
	{
	private:
		const preferences_page_callback::ptr _callback;

		wchar_t _libPath[MAX_PATH];
		wchar_t _interface[33];
		int _port;
	public:
		enum { IDD = IDD_SERVERPREFERENCESPAGE };

	public:
		BEGIN_DDX_MAP(ConfigDialogInstance)
			DDX_TEXT_LEN(IDC_LIBRARYPATH, _libPath, MAX_PATH)
			DDX_UINT_RANGE(IDC_PORT, _port, 0, ((int)std::numeric_limits<UINT16>::max()));
			DDX_TEXT(IDC_NETWORKINTERFACE, _interface) DDX_TEXT_LEN(IDC_NETWORKINTERFACE, _interface, 32)
		END_DDX_MAP()

		BEGIN_MSG_MAP(ConfigDialogInstance)
			MSG_WM_INITDIALOG(OnInit)
			COMMAND_ID_HANDLER(IDC_REFRESH, OnRefresh)
		END_MSG_MAP()

		explicit ConfigDialogInstance(preferences_page_callback::ptr callback);

		BOOL OnInit(CWindow wnd, LPARAM lParam);
		void OnPathChange(UINT foo, int bar, CWindow baz);
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