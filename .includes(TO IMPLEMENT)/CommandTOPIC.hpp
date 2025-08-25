#ifndef COMMANDTOPIC_HPP
#define COMMANDTOPIC_HPP

#include "CommandBase.hpp"
#include "Client.hpp"
#include <vector>
#include <string>

class TopicCommand : public CommandBase {
public:
    TopicCommand();
    ~TopicCommand();
    responseList execute(Client& client, const ParsedMessage& message);
	CommandBase* clone() const;

private:
    // OCF
    TopicCommand(const TopicCommand& other);
    TopicCommand& operator=(const TopicCommand& other);
};

#endif
