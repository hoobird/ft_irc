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
    responseList execute(Client& client, const ParsedMessage& message);
	CommandBase* clone() const;

private:
    // OCF
    PartCommand(const PartCommand& other);
    PartCommand& operator=(const PartCommand& other);
};

#endif
