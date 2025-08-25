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
    responseList execute(Client& client, const ParsedMessage& message);
	CommandBase* clone() const;

private:
    // OCF
    ModeCommand(const ModeCommand& other);
    ModeCommand& operator=(const ModeCommand& other);
};

#endif
