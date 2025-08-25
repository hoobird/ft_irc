#ifndef COMMANDPASS_HPP
#define COMMANDPASS_HPP

#include "CommandBase.hpp"
#include "AuthService.hpp"
#include "Client.hpp"

#include <vector>
#include <string>

class CommandPASS : public CommandBase {
public:
	CommandPASS(const AuthService& authService);
	CommandPASS(const CommandPASS& other);
	~CommandPASS();
	responseList execute(Client& client, const ParsedMessage& message);
	CommandBase* clone() const;

private:
	const AuthService& authService;
	// OCF
	CommandPASS& operator=(const CommandPASS& other);
};

#endif
