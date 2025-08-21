#ifndef COMMANDNICK_HPP
#define COMMANDNICK_HPP

#include "CommandBase.hpp"
#include "Client.hpp"
#include <vector>
#include <string>

class NickCommand : public CommandBase {
public:
	NickCommand();
	~NickCommand();
	void execute(Client* client, const std::vector<std::string>& params, const std::string& trailing);

private:
	// OCF
	NickCommand(const NickCommand& other);
	NickCommand& operator=(const NickCommand& other);
};

#endif
