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
    void execute(Client* client, const std::vector<std::string>& params, const std::string& trailing);

private:
    // OCF
    TopicCommand(const TopicCommand& other);
    TopicCommand& operator=(const TopicCommand& other);
};

#endif
