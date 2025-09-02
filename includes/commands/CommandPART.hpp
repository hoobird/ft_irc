#ifndef COMMANDPART_HPP
#define COMMANDPART_HPP

#include "CommandBase.hpp"
#include "Client.hpp"
#include "DataStore.hpp"

class CommandPART : public CommandBase {
public:
    CommandPART(DataStore& dataStore);
    CommandPART(const CommandPART& other);
    ~CommandPART();
    responseList execute(Client& client, const ParsedMessage& message);
	CommandBase* clone() const;

private:
    DataStore& dataStore;
    // OCF
    CommandPART();
    CommandPART& operator=(const CommandPART& other);
};

#endif
