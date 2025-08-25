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
    responseList execute(Client& client, const ParsedMessage& message);
	CommandBase* clone() const;

private:
    // OCF
    PingCommand(const PingCommand& other);
    PingCommand& operator=(const PingCommand& other);
};

#endif
