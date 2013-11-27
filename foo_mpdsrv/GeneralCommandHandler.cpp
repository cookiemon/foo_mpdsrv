#include "GeneralCommandHandler.h"
#include "MessageSender.h"

namespace foo_mpdsrv
{
	void HandleClose(MessageSender& sender, std::vector<std::string>&)
	{
		sender.CloseConnection();
	}
}