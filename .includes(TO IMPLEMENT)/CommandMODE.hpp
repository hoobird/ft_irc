#ifndef COMMANDMODE_HPP
#define COMMANDMODE_HPP

#include "CommandBase.hpp"
#include "Client.hpp"
#include <vector>
#include <string>

class ModeCommand : public CommandBase {
public:
    ModeCommand();
    ~ModeCommand();
    void execute(Client* client, const std::vector<std::string>& params, const std::string& trailing);

private:
    // OCF
    ModeCommand(const ModeCommand& other);
    ModeCommand& operator=(const ModeCommand& other);
};

#endif
