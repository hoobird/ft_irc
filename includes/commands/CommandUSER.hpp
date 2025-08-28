#ifndef COMMANDUSER_HPP
#define COMMANDUSER_HPP
#include "CommandBase.hpp"
#include "Client.hpp"
#include <vector>
#include <string>

class CommandUSER : public CommandBase {
public:
	CommandUSER(DataStore const & dataStore, std::string serverName);
	CommandUSER(const CommandUSER& other);
	~CommandUSER();
	responseList execute(Client& client, const ParsedMessage& message);
	CommandBase* clone() const;


private:
	const DataStore& dataStore;
	std::string serverName;
	// OCF
	CommandUSER();
	CommandUSER& operator=(const CommandUSER& other);
};

#endif