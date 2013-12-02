#ifndef NETWORKINFORMATION_H
#define NETWORKINFORMATION_H

#include "common.h"
#include <vector>

namespace foo_mpdsrv
{
	class NetworkInformation
	{
	public:
		static const pfc::string8 _defaultPort;
	private:
		RefCounter<NetworkInformation> _counter;
		static WSAData _socketInformation;
		//WSADATA socketdata;
		INT _lastError;
		ADDRINFOA* _addressinfo;

	public:
		NetworkInformation();
		~NetworkInformation();

		void RequestAddressInfo(const pfc::stringp& addr, const pfc::stringp& port);
		void FreeAddressInfo();
		bool IsAddressValid(pfc::string8 addr);
		bool IsPortValid(const pfc::stringp& port);
		ADDRINFO* GetAddressInfo();
		std::vector<tstring> GetValidInterfaces();
	};
}

#endif