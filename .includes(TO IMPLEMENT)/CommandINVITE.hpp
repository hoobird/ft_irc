#ifndef COMMANDINVITE_HPP
#define COMMANDINVITE_HPP

#include "CommandBase.hpp"
#include "Client.hpp"
#include <vector>
#include <string>

class InviteCommand : public CommandBase {
public:
    InviteCommand();
    ~InviteCommand();
    responseList execute(Client& client, const ParsedMessage& message);
	CommandBase* clone() const;

private:
    // OCF
    InviteCommand(const InviteCommand& other);
    InviteCommand& operator=(const InviteCommand& other);
};

#endif
