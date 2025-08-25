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
    responseList execute(Client& client, const ParsedMessage& message);
	CommandBase* clone() const;

private:
    // OCF
    JoinCommand(const JoinCommand& other);
    JoinCommand& operator=(const JoinCommand& other);
};

#endif
