#ifndef COMMANDNICK_HPP
#define COMMANDNICK_HPP

#include "DataStore.hpp"
#include "CommandBase.hpp"
#include "Client.hpp"
#include <vector>
#include <string>

class CommandNICK : public CommandBase {
public:
	CommandNICK(const DataStore &dataStore);
	CommandNICK(const CommandNICK& other);
	~CommandNICK();
	responseList execute(Client& client, const ParsedMessage& message);
	CommandBase* clone() const;

private:
	const DataStore &dataStore;

	bool isValidNickname(std::string nick);
	// OCF
	CommandNICK();
	CommandNICK& operator=(const CommandNICK& other);
};

#endif
