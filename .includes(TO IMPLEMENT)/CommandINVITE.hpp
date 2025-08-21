#ifndef COMMANDINVITE_HPP
#define COMMANDINVITE_HPP

#include "CommandBase.hpp"
#include "Client.hpp"
#include <vector>
#include <string>

class InviteCommand : public CommandBase {
public:
    InviteCommand();
    ~InviteCommand();
    void execute(Client* client, const std::vector<std::string>& params, const std::string& trailing);

private:
    // OCF
    InviteCommand(const InviteCommand& other);
    InviteCommand& operator=(const InviteCommand& other);
};

#endif
