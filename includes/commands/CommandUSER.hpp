#ifndef COMMANDUSER_HPP
#define COMMANDUSER_HPP
#include "CommandBase.hpp"
#include "Client.hpp"
#include <vector>
#include <string>

class CommandUSER : public CommandBase {
public:
	CommandUSER(DataStore const & dataStore);
	CommandUSER(const CommandUSER& other);
	~CommandUSER();
	responseList execute(Client& client, const ParsedMessage& message);
	CommandBase* clone() const;


private:
	const DataStore& dataStore;
	// OCF
	CommandUSER();
	CommandUSER& operator=(const CommandUSER& other);
};

#endif