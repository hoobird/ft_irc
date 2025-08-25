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
    responseList execute(Client& client, const ParsedMessage& message);
	CommandBase* clone() const;

private:
    // OCF
    QuitCommand(const QuitCommand& other);
    QuitCommand& operator=(const QuitCommand& other);
};

#endif
