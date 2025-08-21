#ifndef COMMANDQUIT_HPP
#define COMMANDQUIT_HPP

#include "CommandBase.hpp"
#include "Client.hpp"
#include <vector>
#include <string>

class QuitCommand : public CommandBase {
public:
    QuitCommand();
    ~QuitCommand();
    void execute(Client* client, const std::vector<std::string>& params, const std::string& trailing);

private:
    // OCF
    QuitCommand(const QuitCommand& other);
    QuitCommand& operator=(const QuitCommand& other);
};

#endif
