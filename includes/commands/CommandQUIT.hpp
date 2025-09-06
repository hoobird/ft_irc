#ifndef COMMANDQUIT_HPP
#define COMMANDQUIT_HPP

#include "CommandBase.hpp"
#include "Client.hpp"
#include "DataStore.hpp"
#include "NetworkManager.hpp"

class CommandQUIT : public CommandBase {
public:
    CommandQUIT(DataStore& dataStore, NetworkManager& networkManager);
    CommandQUIT(const CommandQUIT& other);
    ~CommandQUIT();
    responseList execute(Client& client, const ParsedMessage& message);
	CommandBase* clone() const;

private:
    DataStore& dataStore;
    NetworkManager& networkManager;
    // OCF
    CommandQUIT();
    CommandQUIT& operator=(const CommandQUIT& other);
};

#endif
