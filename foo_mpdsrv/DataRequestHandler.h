#ifndef DATAREQUESTHANDLER_H
#define DATAREQUESTHANDLER_H

#include "MessageSender.h"
#include <vector>
#include <string>

namespace foo_mpdsrv
{
	void HandleCurrentsong(MessageSender& caller, std::vector<std::string>& args);
	void HandleLsinfo(MessageSender& caller, std::vector<std::string>& args);
	void HandleOutputs(MessageSender& caller, std::vector<std::string>&);
	void HandleStats(MessageSender& caller, std::vector<std::string>&);
}
#endif