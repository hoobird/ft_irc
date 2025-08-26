#ifndef COMMANDPRIVMSG_HPP
#define COMMANDPRIVMSG_HPP

#include "CommandBase.hpp"
#include "Client.hpp"
#include "DataStore.hpp"

class CommandPRIVMSG : public CommandBase {
public:
    CommandPRIVMSG(const DataStore& dataStore);
    CommandPRIVMSG(const CommandPRIVMSG& other);
    ~CommandPRIVMSG();
    responseList execute(Client& client, const ParsedMessage& message);
	CommandBase* clone() const;

private:
    const  DataStore&   dataStore;
    // OCF
    CommandPRIVMSG();
    CommandPRIVMSG& operator=(const CommandPRIVMSG& other);
};

#endif
