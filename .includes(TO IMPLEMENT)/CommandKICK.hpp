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
    responseList execute(Client& client, const ParsedMessage& message);
	CommandBase* clone() const;

private:
    // OCF
    KickCommand(const KickCommand& other);
    KickCommand& operator=(const KickCommand& other);
};

#endif
