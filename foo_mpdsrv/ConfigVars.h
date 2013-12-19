#ifndef CONFIGVARS_H
#define CONFIGVARS_H

#include "common.h"

namespace foo_mpdsrv
{
	/**
	 * Type of song ids. 256K of memory should be enough for everyone.
	 * So uint64 should be enough for every music library.
	 * @author Cookiemon
	 */
	typedef t_uint64 idtype;
	/**
	 * Default binding address value
	 * @author Cookiemon
	 */
	extern const char* const g_DefaultAddress;
	/**
	 * Default listening port value
	 * @author Cookiemon
	 */
	extern const char* const g_DefaultPort;
	/**
	 * Default library path
	 * @author Cookiemon
	 * @todo sane value
	 * @todo check if needed
	 */
	extern const char* const g_DefaultLibraryPath;
	/**
	 * Default value if mpd should start automatically
	 * @author Cookiemon
	 */
	extern const bool        g_DefaultAutostart;
	/**
	 * Library path setting
	 * @author Cookiemon
	 */
	extern cfg_string g_LibraryRootPath;
	/**
	 * Binding address setting
	 * @author Cookiemon
	 */
	extern cfg_string g_NetworkInterface;
	/**
	 * Listening port setting
	 * @author Cookiemon
	 */
	extern cfg_string g_Port;
	/**
	 * Maximum of currently used id values
	 * Improves assigning a new id value
	 * @author Cookiemon
	 * @todo Not store id as metadata
	 */
	extern cfg_int_t<idtype> g_MaxId;
	/**
	 * Setting if mpd server should automatically listen on startup
	 * @author Cookiemon
	 */
	extern cfg_bool g_Autostart;
	/**
	 * Metadata field that holds the id
	 * @author Cookiemon
	 */
	extern const char* const g_IdString;
	/**
	 * MPD server welcome message
	 * @author Cookiemon
	 * @todo check which version should be reported
	 */
	extern const char* const g_MPDGreeting;
}

#endif