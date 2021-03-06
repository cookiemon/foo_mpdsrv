#include "common.h"
#include "NetworkInformation.h"
#include <WinSock2.h>
#include <iphlpapi.h>

namespace foo_mpdsrv
{
	WSAData NetworkInformation::_socketInformation;

	NetworkInformation::NetworkInformation() : _addressinfo(NULL)
	{
		TRACK_CALL_TEXT("NetworkInformation::NetworkInformation()");
		if(_counter.GetCount() == 1)
		{
			_lastError = WSAStartup(MAKEWORD(2, 0), &_socketInformation);
			if(_lastError != ERROR_SUCCESS)
			{
				Logger(Logger::SEVERE).LogWinError("Could not initialise WSA", _lastError);
				return;
			}
		}
	}

	NetworkInformation::~NetworkInformation()
	{
		TRACK_CALL_TEXT("NetworkInformation::~NetworkInformation()");
		FreeAddressInfo();
		if(_counter.GetCount() == 1)
		{
			_lastError = WSACleanup();
			if(_lastError != ERROR_SUCCESS)
			{
				Logger(Logger::SEVERE).LogWinError("Could not cleanup WSA", _lastError);
			}
		}
	}

	void NetworkInformation::RequestAddressInfo(const pfc::stringp& addr, const pfc::stringp& port)
	{
		TRACK_CALL_TEXT("NetworkInformation::RequestAddressInfo()");
		if(_addressinfo != NULL) { FreeAddressInfo(); }

		ADDRINFOA hints;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = PF_UNSPEC; // Deprecated?
		hints.ai_socktype = SOCK_STREAM; // TCP Socket
		hints.ai_flags = AI_PASSIVE; // Bind to local address

		_lastError = getaddrinfo(addr.get_ptr(), port.get_ptr(), &hints, &_addressinfo);
		if(_lastError != ERROR_SUCCESS)
		{
			Logger(Logger::SEVERE).LogWinError("Could not retrieve address information", _lastError);
			return;
		}
	}

	void NetworkInformation::FreeAddressInfo()
	{
		TRACK_CALL_TEXT("NetworkInformation::FreeAddressInfo()");
		if(_addressinfo != NULL)
			freeaddrinfo(_addressinfo);
		_addressinfo = NULL;
	}

	bool NetworkInformation::IsAddressValid(pfc::string8 addr)
	{
		TRACK_CALL_TEXT("NetworkInformation::IsAddressValis()");
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
		TRACK_CALL_TEXT("NetworkInformation::IsPortValid()");
		char* end;
		// TODO: Improve. Port range is 16 bit
		long portNum = strtol(port.get_ptr(), &end, 10);
		if(*end != '\0' || port.length() == 0 || portNum > std::numeric_limits<unsigned short>::max())
			return false;
		else
			return true;
	}
	
	ADDRINFOA* NetworkInformation::GetAddressInfo()
	{
		TRACK_CALL_TEXT("NetworkInformation::GetAddressInfo()");
		return _addressinfo;
	}
}