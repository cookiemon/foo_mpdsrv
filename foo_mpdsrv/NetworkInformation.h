#ifndef NETWORKINFORMATION_H
#define NETWORKINFORMATION_H

#include "common.h"
#include "RefCounter.h"
#include "StringUtil.h"
#include <vector>
#include <WS2tcpip.h>

namespace foo_mpdsrv
{
	/**
	 * Class handling network information requests
	 * @author Cookiemon
	 */
	class NetworkInformation
	{
	private:
		// Counter used for WSAStartup and WSACleanup calls
		RefCounter<NetworkInformation> _counter;
		static WSAData _socketInformation;
		int _lastError;
		ADDRINFOA* _addressinfo;

	public:
		/**
		 * Calls WSAStartup if the first instance after
		 * cleanup or program startup is created
		 * @author Cookiemon
		 */
		NetworkInformation();
		/**
		 * Calls WSACleanup if the last instance is deleted
		 * and frees the ADDRINFO structure
		 * @author Cookiemon
		 */
		~NetworkInformation();
		
		/**
		 * Refreshes the stored address info structure
		 * @author Cookiemon
		 * @param addr Address for which address information should be obtained
		 * @param port Port for which address information should be obtained
		 */
		void RequestAddressInfo(const pfc::stringp& addr, const pfc::stringp& port);
		/**
		 * Frees the stored address information
		 * @author Cookiemon
		 */
		void FreeAddressInfo();
		/**
		 * Checks if the given string is a valid IPv4 oder IPv6 address
		 * @author Cookiemon
		 * @param addr String to check
		 * @return true iff addr is valid IPv4 or IPv6 address
		 */
		bool IsAddressValid(pfc::string8 addr);
		/**
		 * Checks if the given string is a valid port
		 * @author Cookiemon
		 * @param port Port number to check
		 * @return true iff port is a valid port number
		 */
		bool IsPortValid(const pfc::stringp& port);
		/**
		 * Returns first ADDRINFO structure
		 * @author Cookiemon
		 * @return address information
		 */
		ADDRINFO* GetAddressInfo();
	};
}

#endif