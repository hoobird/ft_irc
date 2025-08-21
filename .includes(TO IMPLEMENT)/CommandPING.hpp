#ifndef COMMANDPING_HPP
#define COMMANDPING_HPP

#include "CommandBase.hpp"
#include "Client.hpp"
#include <vector>
#include <string>

class PingCommand : public CommandBase {
public:
    PingCommand();
    ~PingCommand();
    void execute(Client* client, const std::vector<std::string>& params, const std::string& trailing);

private:
    // OCF
    PingCommand(const PingCommand& other);
    PingCommand& operator=(const PingCommand& other);
};

#endif
