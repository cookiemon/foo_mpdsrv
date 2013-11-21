#include "common.h"
#include "NetworkInformation.h"

inline void popupNetworkError(const char* message, int errorNum = -1)
{
	if(errorNum == -1)
		errorNum = WSAGetLastError();
	//console::formatter form;
	//form << message << " (" << errorNum << ", " << gai_strerrorA(errorNum) << ")";
	//popup_message::g_show(form, PLUGINNAME, popup_message::icon_error);
	foo_mpdsrv::Logger log(foo_mpdsrv::Logger::SEVERE);
	log.Log(message);
	log.Log(" (");
	log.Log(errorNum);
	log.Log(", ");
	log.Log(gai_strerrorA(errorNum));
	log.Log(")\n");
}

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
				popupNetworkError("Could not initialize WinSocks", _lastError);
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
				popupNetworkError("Could not cleanup WinSocks", _lastError);
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

		_lastError = getaddrinfo("0.0.0.0", port.get_ptr(), &hints, &_addressinfo);
		if(_lastError != ERROR_SUCCESS)
		{
			popupNetworkError("Could not retrieve address information", _lastError);
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
}