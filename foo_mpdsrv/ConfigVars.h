#ifndef CONFIGVARS_H
#define CONFIGVARS_H

#include "common.h"

namespace foo_mpdsrv
{
	typedef t_uint64 idtype;
	extern const char* const g_DefaultPort;
	extern cfg_string g_LibraryRootPath;
	extern cfg_string g_NetworkInterface;
	extern cfg_string g_Port;
	extern cfg_int_t<idtype> g_MaxId;
	extern const char* const g_IdString;
	extern const char* const g_MPDGreeting;
}

#endif