// dllmain.cpp : Defines the entry point for the DLL application.
#include "common.h"
#include "ConnectionListener.h"
#include "ConfigVars.h"
#include "LibraryConsistencyCheck.h"

namespace foo_mpdsrv
{
	DECLARE_COMPONENT_VERSION("MPD Server Component", "0.1 Alpha", "Makes foobar remote controllable through MPD Network interface");
	VALIDATE_COMPONENT_FILENAME("foo_mpdsrv.dll");
	const char* PLUGINNAME = "MPD Server";
	const wchar_t* PLUGINNAMEW = L"MPD Server";
}

namespace foo_mpdsrv
{
	std::auto_ptr<ConnectionListener> listener;

	class init : public initquit
	{
	public:
		void on_init()
		{
			TRACK_CALL_TEXT("init::on_init()");
			if(g_Autostart)
			{
				listener.reset(new ConnectionListener());
				listener->StartListening(g_NetworkInterface, g_Port);
			}
		}
		void on_quit()
		{
			TRACK_CALL_TEXT("init::on_quit()");
			if(listener.get())
			{
				listener->StopListening();
				listener.reset(NULL);
			}
		}
	};

	initquit_factory_t<init> lalelu;

	class mainmenu_command_connect : public mainmenu_commands
	{
	public:
		// {A53ABD2F-1BFA-4D63-B1EA-A65429A44198}
		static const GUID _command_guid;
		enum {
			cmd_connect,
			cmd_disconnect,
			cmd_reconnect,
			cmd_library,
			cmd_total
		};
	
		GUID get_parent() { return _command_guid; }
		t_uint32 get_command_count() { return cmd_total; }

		GUID get_command(t_uint32 idx)
		{
			TRACK_CALL_TEXT("mainmenu_command_connect::get_command()");
			// {44E8D987-5CFE-4A7C-8FC4-CD02AA053F7A}
			static const GUID guid_cmd_connect = { 0x44e8d987, 0x5cfe, 0x4a7c, { 0x8f, 0xc4, 0xcd, 0x2, 0xaa, 0x5, 0x3f, 0x7a } };
			// {84036C44-97E6-4F76-BEA7-FE5DFCCD5B32}
			static const GUID guid_cmd_disconnect = { 0x84036c44, 0x97e6, 0x4f76, { 0xbe, 0xa7, 0xfe, 0x5d, 0xfc, 0xcd, 0x5b, 0x32 } };
			// {DFFD60E5-F866-4033-AEF3-9B8DAB985901}
			static const GUID guid_cmd_reconnect = { 0xdffd60e5, 0xf866, 0x4033, { 0xae, 0xf3, 0x9b, 0x8d, 0xab, 0x98, 0x59, 0x1 } };
			// {8034B308-9FEB-4560-BA5A-72A12AC972A6}
			static const GUID guid_cmd_library = { 0x8034b308, 0x9feb, 0x4560, { 0xba, 0x5a, 0x72, 0xa1, 0x2a, 0xc9, 0x72, 0xa6 } };

			switch(idx)
			{
			case cmd_connect:
				return guid_cmd_connect;
			case cmd_disconnect:
				return guid_cmd_disconnect;
			case cmd_reconnect:
				return guid_cmd_reconnect;
			case cmd_library:
				return guid_cmd_library;
			default:
				// PANIC!!
				uBugCheck();
			}
		}

		void get_name(t_uint32 idx, pfc::string_base& out)
		{
			TRACK_CALL_TEXT("mainmenu_command_connect::get_name()");
			switch(idx)
			{
			case cmd_connect:
				out = "Start listening";
				break;
			case cmd_disconnect:
				out = "Stop listening";
				break;
			case cmd_reconnect:
				out = "Restart listening";
				break;
			case cmd_library:
				out = "Check Library";
				break;
			case cmd_total:
				out = "This is madness!";
				break;
			default:
				// PANIC!!
				uBugCheck();
			}
		}

		bool get_display(t_uint32 idx, pfc::string_base& out, t_uint32& flags)
		{
			TRACK_CALL_TEXT("mainmenu_command_connect::get_display()");
			get_name(idx, out);
			switch(idx)
			{
			case cmd_connect:
				flags = listener.get() != NULL ? flag_disabled:0;
				break;
			case cmd_disconnect:
				flags = listener.get() == NULL ? flag_disabled:0;
				break;
			case cmd_reconnect:
			case cmd_library:
				break;
			default:
				uBugCheck();
			}
			return true;
		}

		bool get_description(t_uint32 idx, pfc::string_base& out)
		{
			TRACK_CALL_TEXT("mainmenu_command_connect::get_description()");
			switch(idx)
			{
			case cmd_connect:
				out = "Starts listening";
				return true;
			case cmd_disconnect:
				out = "Stops listening";
				return true;
			case cmd_reconnect:
				out = "Restarts listening";
				return true;
			case cmd_library:
				out = "Check library";
				return true;
			case cmd_total:
				out = "This is spartaaaaaaaaa!";
				return true;
			default:
				// PANIC!!
				uBugCheck();
			}
		}

		virtual void execute(t_uint32 idx,service_ptr_t<service_base> unused)
		{
			TRACK_CALL_TEXT("mainmenu_command_connect::execute()");
			switch(idx)
			{
			case cmd_connect:
				listener.reset(new ConnectionListener());
				listener->StartListening(g_NetworkInterface, g_Port);
				break;
			case cmd_disconnect:
				if(listener.get())
				{
					listener->StopListening();
					listener.reset(NULL);
				}
				break;
			case cmd_reconnect:
				if(listener.get())
				{
					listener->RefreshConnection(g_NetworkInterface, g_Port);
				}
				else
				{
					listener.reset(new ConnectionListener());
					listener->StartListening(g_NetworkInterface, g_Port);
				}
				break;
			case cmd_library:
				{
					service_ptr_t<threaded_process_callback> bla(new LibraryConsistencyWorker());
					threaded_process::g_run_modeless(bla,
						threaded_process::flag_show_progress | threaded_process::flag_show_item | threaded_process::flag_show_abort,
						core_api::get_main_window(),
						"Checking for database consistency");
				}
				break;
			default:
				// PANIC!!
				uBugCheck();
			}
		}
	};

	// {A53ABD2F-1BFA-4D63-B1EA-A65429A44198}
	const GUID mainmenu_command_connect::_command_guid = { 0xa53abd2f, 0x1bfa, 0x4d63, { 0xb1, 0xea, 0xa6, 0x54, 0x29, 0xa4, 0x41, 0x98 } };

	mainmenu_group_popup_factory mainmenugroup(mainmenu_command_connect::_command_guid,
		mainmenu_groups::file, static_cast<t_uint32>(mainmenu_commands::sort_priority_dontcare), "MPD Server");

	mainmenu_commands_factory_t<mainmenu_command_connect> mainmenucommand;

}