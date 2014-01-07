#include "MessageTransporter.h"

namespace foo_mpdsrv
{
	MessageTransporter::MessageTransporter(SOCKET connection)
	{
		TRACK_CALL_TEXT("MessageTransporter::MessageTransporter()");
		_sock = accept(connection, NULL, NULL);
	}

	MessageTransporter::MessageTransporter(MessageTransporter&& right)
	{
		TRACK_CALL_TEXT("MessageTransporter::MessageTransporter(&&)");
		_sock = right._sock;
		right._sock = static_cast<SOCKET>(SOCKET_ERROR);
	}

	MessageTransporter::~MessageTransporter()
	{
		TRACK_CALL_TEXT("MessageTransporter::~MessageTransporter()");
		if(_sock != SOCKET_ERROR)
		{
			Close();
		}
	}

	bool MessageTransporter::SendAnswer(const std::string& message)
	{
		size_t numBytes = message.length();
		return SendBytes(message.c_str(), numBytes);
	}

	bool MessageTransporter::SendAnswer(const pfc::string8& message)
	{
		size_t numBytes = message.get_length();
		return SendBytes(message.get_ptr(), numBytes);
	}

	bool MessageTransporter::SendAnswer(std::istream& message)
	{
		std::string buf;
		bool retVal = true;
		while(std::getline(message, buf))
			retVal = retVal && SendAnswer(buf + '\n');
		return retVal;
	}

	bool MessageTransporter::SendAnswer(const char* message)
	{
		size_t numBytes = strlen(message);
		return SendBytes(message, numBytes);
	}

	bool MessageTransporter::SendBytes(const char* buf, int numBytes)
	{
		if(_sock == static_cast<SOCKET>(SOCKET_ERROR)) { return false; }
		if(numBytes == 0) { return true; }

		WaitUntilReady();
		
		int bytesSend = send(_sock, buf, numBytes, 0);
		if(bytesSend == static_cast<SOCKET>(SOCKET_ERROR))
		{
			int lastError = WSAGetLastError();
			if(lastError != WSAEWOULDBLOCK)
			{
				Logger(Logger::SEVERE).LogWinError("Could not send message", lastError);
				return false;
			}
			else
			{
				Logger(Logger::SEVERE) << "send returned WSAEWOULDBLOCK";
			}
		}
		else if(numBytes != bytesSend)
		{
			Logger(Logger::SEVERE) << "Not all bytes were sent";
			return false;
		}
		Logger(Logger::FINEST).Log("O: ").Write(buf, numBytes);
		return true;
	}

	void MessageTransporter::WaitUntilReady()
	{
		int lastError = 0;
		fd_set fds;
		fds.fd_count = 1;
		fds.fd_array[0] = _sock;
		lastError = select(0, NULL, &fds, NULL, NULL);
		if(lastError == SOCKET_ERROR)
		{
			lastError = WSAGetLastError();
			Logger(Logger::SEVERE).LogWinError("Could not select socket", lastError);
		}
	}

	void MessageTransporter::Close()
	{
		TRACK_CALL_TEXT("MessageTransporter::Close()");
		closesocket(_sock);
		_sock = SOCKET_ERROR;
	}
}