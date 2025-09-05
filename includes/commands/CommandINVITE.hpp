#ifndef COMMANDINVITE_HPP
#define COMMANDINVITE_HPP

#include "CommandBase.hpp"
#include "Client.hpp"
#include "DataStore.hpp"

class CommandINVITE : public CommandBase {
public:
    CommandINVITE(DataStore& dataStore);
    CommandINVITE(const CommandINVITE& other);
    ~CommandINVITE();
    responseList execute(Client& client, const ParsedMessage& message);
	CommandBase* clone() const;

private:
    DataStore& dataStore;
    // OCF
    CommandINVITE();
    CommandINVITE& operator=(const CommandINVITE& other);
};

#endif
