#include "ConfigVars.h"

namespace foo_mpdsrv
{
	// {C31F0D9E-5128-4596-8221-CE7B2655982E}
	static const GUID RootPathGUID = { 0xc31f0d9e, 0x5128, 0x4596, { 0x82, 0x21, 0xce, 0x7b, 0x26, 0x55, 0x98, 0x2e } };
	// {25B60F03-7C59-4246-9919-072C4EF94E55}
	static const GUID NetworkInterfaceGUID = { 0x25b60f03, 0x7c59, 0x4246, { 0x99, 0x19, 0x7, 0x2c, 0x4e, 0xf9, 0x4e, 0x55 } };
	// {DF1912C4-4A98-4EC9-9EA9-5D8FDBAA4073}
	static const GUID PortGUID = { 0xdf1912c4, 0x4a98, 0x4ec9, { 0x9e, 0xa9, 0x5d, 0x8f, 0xdb, 0xaa, 0x40, 0x73 } };
	// {7D1F4907-9C66-43EB-8CCF-6D65ED9350C3}
	static const GUID IdGUID = { 0x7d1f4907, 0x9c66, 0x43eb, { 0x8c, 0xcf, 0x6d, 0x65, 0xed, 0x93, 0x50, 0xc3 } };

	cfg_string g_LibraryRootPath(RootPathGUID, "");
	cfg_string g_NetworkInterface(NetworkInterfaceGUID, "0.0.0.0");
	cfg_string g_Port(PortGUID, g_DefaultPort);
	cfg_int_t<idtype> g_MaxId(IdGUID, 1);

	const char* const g_DefaultPort = "6600";	
	const char* const g_IdString = "MPDID";
	const char* const g_MPDGreeting = "OK MPD 0.12.2\n";
}