#ifndef COMMANDJOIN_HPP
#define COMMANDJOIN_HPP

#include "CommandBase.hpp"
#include "Client.hpp"
#include "DataStore.hpp"

class CommandJOIN : public CommandBase {
public:
    CommandJOIN(DataStore& dataStore, const Channel& channel);
    CommandJOIN(const CommandJOIN& other);
    ~CommandJOIN();
    responseList execute(Client& client, const ParsedMessage& message);
    CommandBase* clone() const;

private:
    DataStore& dataStore;
    const Channel& channel;
    std::map<std::string, std::string> mapChannelKey;
    // OCF
    CommandJOIN();
    CommandJOIN& operator=(const CommandJOIN& other);
};

#endif
