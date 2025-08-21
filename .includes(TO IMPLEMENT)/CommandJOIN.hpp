#ifndef COMMANDJOIN_HPP
#define COMMANDJOIN_HPP

#include "CommandBase.hpp"
#include "Client.hpp"
#include <vector>
#include <string>

class JoinCommand : public CommandBase {
public:
    JoinCommand();
    ~JoinCommand();
    void execute(Client* client, const std::vector<std::string>& params, const std::string& trailing);

private:
    // OCF
    JoinCommand(const JoinCommand& other);
    JoinCommand& operator=(const JoinCommand& other);
};

#endif
