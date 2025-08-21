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
    void execute(Client* client, const std::vector<std::string>& params, const std::string& trailing);

private:
    // OCF
    PrivMsgCommand(const PrivMsgCommand& other);
    PrivMsgCommand& operator=(const PrivMsgCommand& other);
};

#endif
