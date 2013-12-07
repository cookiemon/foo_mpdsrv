#include "ConfigDialog.h"
#include "ConfigVars.h"
#include "common.h"
#include "NetworkInformation.h"
#include <vector>

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
	
	bool ConfigDialogInstance::HasChanged()
	{
		pfc::string root;
		pfc::string networkIF;
		pfc::string port;
		bool autostart;
		root = uGetDlgItemText(m_hWnd, IDC_LIBRARYPATH);
		port = uGetDlgItemText(m_hWnd, IDC_PORT);
		networkIF = uGetDlgItemText(m_hWnd, IDC_NETWORKINTERFACE);
		CCheckBox cb(GetDlgItem(IDC_AUTOSTART));
		autostart = cb.GetCheck() != BST_CHECKED;
		return root != g_LibraryRootPath || port != g_Port || networkIF != g_NetworkInterface || autostart == g_Autostart;
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
		pfc::string libPath = uGetDlgItemText(m_hWnd, IDC_LIBRARYPATH);
		g_LibraryRootPath = libPath.get_ptr();
		pfc::string port = uGetDlgItemText(m_hWnd, IDC_PORT);
		g_Port = port.get_ptr();
		pfc::string interf = uGetDlgItemText(m_hWnd, IDC_NETWORKINTERFACE);
		g_NetworkInterface = interf.get_ptr();
		CCheckBox cb(GetDlgItem(IDC_AUTOSTART));
		g_Autostart = cb.GetCheck() != BST_UNCHECKED;
		// Error check
	}

	void ConfigDialogInstance::reset()
	{
		g_Port = g_DefaultPort;
		g_NetworkInterface = g_DefaultAddress;
		g_LibraryRootPath = g_DefaultLibraryPath;
		g_Autostart = g_DefaultAutostart;
		ShowSavedValues();
	}

	void ConfigDialogInstance::ShowSavedValues()
	{
		uSetDlgItemText(m_hWnd, IDC_PORT, g_Port);
		uSetDlgItemText(m_hWnd, IDC_NETWORKINTERFACE, g_NetworkInterface);
		uSetDlgItemText(m_hWnd, IDC_LIBRARYPATH, g_LibraryRootPath);
		CCheckBox cb(GetDlgItem(IDC_AUTOSTART));
		cb.SetCheck(g_Autostart!=false?BST_CHECKED:BST_UNCHECKED);
	}

	void ConfigDialogInstance::OnChangedItem(UINT wNotifyCode, int ctrl, HWND hWnd)
	{
		OnChange();
		SetMsgHandled(FALSE);
	}
}