#ifndef COMMANDTOPIC_HPP
#define COMMANDTOPIC_HPP

#include "CommandBase.hpp"
#include "Client.hpp"
#include "DataStore.hpp"

class CommandTOPIC : public CommandBase {
public:
    CommandTOPIC(const DataStore& dataStore);
    CommandTOPIC(const CommandTOPIC& other);
    ~CommandTOPIC();
    responseList execute(Client& client, const ParsedMessage& message);
	CommandBase* clone() const;

private:
    const DataStore& dataStore;
    // OCF
    CommandTOPIC();
    CommandTOPIC& operator=(const CommandTOPIC& other);
};

#endif
