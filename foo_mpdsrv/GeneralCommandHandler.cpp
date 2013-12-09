#include "GeneralCommandHandler.h"
#include "MessageSender.h"

namespace foo_mpdsrv
{
	void HandleClose(MessageSender& sender, std::vector<std::string>&)
	{
		TRACK_CALL_TEXT("HandleClose()");
		sender.CloseConnection();
	}
}