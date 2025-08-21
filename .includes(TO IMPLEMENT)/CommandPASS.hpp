#ifndef COMMANDPASS_HPP
#define COMMANDPASS_HPP

#include "CommandBase.hpp"
#include "Client.hpp"
#include <vector>
#include <string>

class PassCommand : public CommandBase {
public:
	PassCommand();
	~PassCommand();
	void execute(Client* client, const std::vector<std::string>& params, const std::string& trailing);

private:
	// OCF
	PassCommand(const PassCommand& other);
	PassCommand& operator=(const PassCommand& other);
};

#endif
