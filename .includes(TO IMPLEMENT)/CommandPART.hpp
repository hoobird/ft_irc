#ifndef COMMANDPART_HPP
#define COMMANDPART_HPP

#include "CommandBase.hpp"
#include "Client.hpp"
#include <vector>
#include <string>

class PartCommand : public CommandBase {
public:
    PartCommand();
    ~PartCommand();
    void execute(Client* client, const std::vector<std::string>& params, const std::string& trailing);

private:
    // OCF
    PartCommand(const PartCommand& other);
    PartCommand& operator=(const PartCommand& other);
};

#endif
