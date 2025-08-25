#ifndef COMMANDPING_HPP
#define COMMANDPING_HPP

#include "CommandBase.hpp"
#include "Client.hpp"

class CommandPING : public CommandBase {
public:
    CommandPING(const std::string& serverName);
    CommandPING(const CommandPING& other);
    ~CommandPING();
    responseList execute(Client& client, const ParsedMessage& message);
    CommandBase* clone() const;

private:
    const std::string& serverName;
    CommandPING();
    CommandPING& operator=(const CommandPING& other);
};

#endif
