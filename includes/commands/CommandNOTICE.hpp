#ifndef COMMANDNOTICE_HPP
#define COMMANDNOTICE_HPP

#include "CommandBase.hpp"
#include "Client.hpp"
#include "DataStore.hpp"
#include "bonus/CTCPHandler.hpp"

class CommandNOTICE : public CommandBase {
public:
    CommandNOTICE(const DataStore& dataStore);
    CommandNOTICE(const CommandNOTICE& other);
    ~CommandNOTICE();
    responseList execute(Client& client, const ParsedMessage& message);
	CommandBase* clone() const;

    responseList handleClientRecipient(Client &sender, const std::string &recipientNick, const ParsedMessage &message);
    responseList handleChannelRecipient(Client &sender, const std::string &channelName, const ParsedMessage &message);

private:
    const DataStore&  dataStore;
    // OCF
    CommandNOTICE();
    CommandNOTICE& operator=(const CommandNOTICE& other);
};

#endif
