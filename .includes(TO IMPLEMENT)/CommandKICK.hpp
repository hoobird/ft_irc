#ifndef COMMANDKICK_HPP
#define COMMANDKICK_HPP

#include "CommandBase.hpp"
#include "Client.hpp"
#include <vector>
#include <string>

class KickCommand : public CommandBase {
public:
    KickCommand();
    ~KickCommand();
    void execute(Client* client, const std::vector<std::string>& params, const std::string& trailing);

private:
    // OCF
    KickCommand(const KickCommand& other);
    KickCommand& operator=(const KickCommand& other);
};

#endif
