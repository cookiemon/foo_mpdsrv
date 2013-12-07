#ifndef CONFIGVARS_H
#define CONFIGVARS_H

#include "common.h"

namespace foo_mpdsrv
{
	typedef t_uint64 idtype;
	extern const char* const g_DefaultAddress;
	extern const char* const g_DefaultPort;
	extern const char* const g_DefaultLibraryPath;
	extern const bool        g_DefaultAutostart;
	extern cfg_string g_LibraryRootPath;
	extern cfg_string g_NetworkInterface;
	extern cfg_string g_Port;
	extern cfg_int_t<idtype> g_MaxId;
	extern cfg_bool g_Autostart;
	extern const char* const g_IdString;
	extern const char* const g_MPDGreeting;
}

#endif