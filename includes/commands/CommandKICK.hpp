#ifndef COMMANDKICK_HPP
#define COMMANDKICK_HPP

#include "CommandBase.hpp"
#include "Client.hpp"
#include <vector>
#include <string>

class CommandKICK : public CommandBase {
public:
    CommandKICK(DataStore& dataStore);
    CommandKICK(const CommandKICK& other);
    ~CommandKICK();
    responseList execute(Client& client, const ParsedMessage& message);
	CommandBase* clone() const;

private:
    DataStore& dataStore;
    // OCF
    CommandKICK();
    CommandKICK& operator=(const CommandKICK& other);
};

#endif
