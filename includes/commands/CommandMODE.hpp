#ifndef COMMANDMODE_HPP
#define COMMANDMODE_HPP

#include "CommandBase.hpp"
#include "Client.hpp"
#include "DataStore.hpp"
#include <vector>
#include <string>

class CommandMODE : public CommandBase {
public:
    CommandMODE(DataStore const & dataStore);
    CommandMODE(const CommandMODE& other);
    ~CommandMODE();
    responseList execute(Client& client, const ParsedMessage& message);
	CommandBase* clone() const;

private:
    DataStore const &dataStore;
    // OCF
    CommandMODE();
    CommandMODE& operator=(const CommandMODE& other);
};

#endif
