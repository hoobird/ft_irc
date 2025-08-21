#ifndef COMMANDUSER_HPP
#define COMMANDUSER_HPP
#include "CommandBase.hpp"
#include "Client.hpp"
#include <vector>
#include <string>

class UserCommand : public CommandBase {
public:
	UserCommand();
	~UserCommand();
	void execute(Client* client, const std::vector<std::string>& params, const std::string& trailing);

private:
	// OCF
	UserCommand(const UserCommand& other);
	UserCommand& operator=(const UserCommand& other);
};

#endif