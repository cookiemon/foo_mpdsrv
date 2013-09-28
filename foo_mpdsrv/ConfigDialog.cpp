#include "ConfigDialog.h"
#include "ConfigVars.h"

namespace foo_mpdsrv
{
	// {5F37D2BC-F9F2-4930-8B4B-555CDD546946}
	const GUID ConfigDialog::ConfigDialogGUID = { 0x5f37d2bc, 0xf9f2, 0x4930, { 0x8b, 0x4b, 0x55, 0x5c, 0xdd, 0x54, 0x69, 0x46 } };
	const char* ConfigDialog::ConfigDialogName = "MPD Server";

	preferences_page_factory_t<ConfigDialog> confFac;

	const char* ConfigDialog::get_name()
	{
		return ConfigDialogName;
	}

	GUID ConfigDialog::get_guid()
	{
		return ConfigDialogGUID;
	}

	GUID ConfigDialog::get_parent_guid()
	{
		return preferences_page::guid_tools;
	}

	ConfigDialogInstance::ConfigDialogInstance(preferences_page_callback::ptr callback) : _callback(callback)
	{
	}

	BOOL ConfigDialogInstance::OnInit(CWindow wnd, LPARAM lParam)
	{
		ShowSavedValues();
		return FALSE;
	}
	
	void ConfigDialogInstance::OnPathChange(UINT foo, int bar, CWindow baz)
	{
		OnChange();
	}

	bool ConfigDialogInstance::HasChanged()
	{
		pfc::string root;
		pfc::string networkIF;
		pfc::string port;
		root = uGetDlgItemText(m_hWnd, IDC_LIBRARYPATH);
		port = uGetDlgItemText(m_hWnd, IDC_PORT);
		return root != g_LibraryRootPath || port != g_Port;
	}

	void ConfigDialogInstance::OnChange()
	{
		_callback->on_state_changed();
	}

	t_uint32 ConfigDialogInstance::get_state()
	{
		return preferences_state::resettable
			| (HasChanged()?preferences_state::changed:0);
	}

	void ConfigDialogInstance::apply()
	{
		pfc::string str;
		BOOL retval;
		pfc::string port;
		str = uGetDlgItemText(m_hWnd, IDC_LIBRARYPATH);
		g_LibraryRootPath = str.get_ptr();
		port = uGetDlgItemText(m_hWnd, IDC_PORT);
		g_Port = port.get_ptr();
		// Error check
	}

	void ConfigDialogInstance::reset()
	{
		g_Port = g_DefaultPort;
		g_NetworkInterface = "0.0.0.0";
		g_LibraryRootPath = "";
		ShowSavedValues();
	}

	void ConfigDialogInstance::ShowSavedValues()
	{
		uSetDlgItemText(m_hWnd, IDC_LIBRARYPATH, g_LibraryRootPath);
		uSetDlgItemText(m_hWnd, IDC_PORT, g_Port);
	}
}