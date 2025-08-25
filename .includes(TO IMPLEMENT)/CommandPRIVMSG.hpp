#ifndef COMMANDPRIVMSG_HPP
#define COMMANDPRIVMSG_HPP

#include "CommandBase.hpp"
#include "Client.hpp"
#include <vector>
#include <string>

class PrivMsgCommand : public CommandBase {
public:
    PrivMsgCommand();
    ~PrivMsgCommand();
    responseList execute(Client& client, const ParsedMessage& message);
	CommandBase* clone() const;

private:
    // OCF
    PrivMsgCommand(const PrivMsgCommand& other);
    PrivMsgCommand& operator=(const PrivMsgCommand& other);
};

#endif
