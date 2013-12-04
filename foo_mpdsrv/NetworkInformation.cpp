#include "common.h"
#include "NetworkInformation.h"
#include <iphlpapi.h>

namespace foo_mpdsrv
{

	WSAData NetworkInformation::_socketInformation;
	const pfc::string8 NetworkInformation::_defaultPort("6600");

	NetworkInformation::NetworkInformation() : _addressinfo(NULL)
	{
		if(_counter.getCount() == 1)
		{
			_lastError = WSAStartup(MAKEWORD(2, 0), &_socketInformation);
			if(_lastError != ERROR_SUCCESS)
			{
				Logger log(Logger::SEVERE);
				log.LogWinError("Could not initialise WSA", _lastError);
				return;
			}
		}
	}

	NetworkInformation::~NetworkInformation()
	{
		FreeAddressInfo();
		if(_counter.getCount() == 1)
		{
			_lastError = WSACleanup();
			if(_lastError != ERROR_SUCCESS)
			{
				Logger log(Logger::SEVERE);
				log.LogWinError("Could not cleanup WSA", _lastError);
			}
		}
	}

	void NetworkInformation::RequestAddressInfo(const pfc::stringp& addr, const pfc::stringp& port)
	{
		if(_addressinfo != NULL) { FreeAddressInfo(); }

		ADDRINFOA hints;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = PF_UNSPEC; // Deprecated?
		hints.ai_socktype = SOCK_STREAM; // TCP Socket
		hints.ai_flags = AI_PASSIVE; // Bind to local address

		_lastError = getaddrinfo(addr.get_ptr(), port.get_ptr(), &hints, &_addressinfo);
		if(_lastError != ERROR_SUCCESS)
		{
			Logger log(Logger::SEVERE);
			log.LogWinError("Could not retrieve address information", _lastError);
			return;
		}
	}

	void NetworkInformation::FreeAddressInfo()
	{
		if(_addressinfo != NULL)
			FreeAddrInfoA(_addressinfo);
		_addressinfo = NULL;
	}

	bool NetworkInformation::IsAddressValid(pfc::string8 addr)
	{
		union
		{
			sockaddr_in ipv4;
			sockaddr_in6 ipv6;
		} buf;
		INT retval;
		INT size = sizeof(buf.ipv4);
		// Help me, I killed a cat! :(
		retval = WSAStringToAddressA(const_cast<char*>(addr.get_ptr()),
			PF_INET, NULL,
			reinterpret_cast<SOCKADDR*>(&buf.ipv4), &size);
		if(retval == SOCKET_ERROR)
		{
			size = sizeof(buf.ipv6);
			retval = WSAStringToAddressA(const_cast<char*>(addr.get_ptr()),
				PF_INET6, NULL,
				reinterpret_cast<SOCKADDR*>(&buf.ipv4), &size);
			if(retval == SOCKET_ERROR)
				return false;
		}
		return true;
	}

	bool NetworkInformation::IsPortValid(const pfc::stringp& port)
	{
		char* end;
		long portNum = strtol(port.get_ptr(), &end, 10);
		if(*end != '\0' || port.length() == 0 || portNum > std::numeric_limits<unsigned short>::max())
			return false;
		else
			return true;
	}
	
	ADDRINFOA* NetworkInformation::GetAddressInfo()
	{
		return _addressinfo;
	}

	std::vector<tstring> NetworkInformation::GetValidInterfaces()
	{
		HRESULT hr;
		ULONG size = 0;
		
		GetAdaptersAddresses(AF_UNSPEC, 0, NULL, NULL, &size);
		if(size == 0)
			return std::vector<tstring>();
		
		IP_ADAPTER_ADDRESSES* addr = NULL;
		do
		{
			if(addr != NULL)
				delete[] addr;
			addr = new IP_ADAPTER_ADDRESSES[size];
			hr = GetAdaptersAddresses(AF_UNSPEC, 0, NULL, addr, &size);
		} while(hr == ERROR_BUFFER_OVERFLOW);
		if(!SUCCEEDED(hr))
		{
			DWORD err = GetLastError();
			Logger log(Logger::SEVERE);
			log.Log("Could not retrieve network adapters: ");
			log.Log(GetErrString(err));
			return std::vector<tstring>();
		}
		IP_ADAPTER_ADDRESSES* cur = addr;
		std::vector<tstring> retVal;
		do
		{
			retVal.push_back(cur->FriendlyName);
			cur = cur->Next;
		} while(cur->Next != NULL);
		return retVal;
	}
}