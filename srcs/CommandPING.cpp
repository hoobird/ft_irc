#include "AllCommands.hpp"

CommandPING::CommandPING(const std::string& serverName)
: serverName(serverName) {

}

CommandPING::CommandPING(const CommandPING& other)
: CommandBase(), serverName(other.serverName) {
    (void)other;
}

CommandPING::~CommandPING() {

}

responseList CommandPING::execute(Client& client, const ParsedMessage& message) {

    responseList responses;
    if (message.parameters.size() < 1) {
        // ERR_NEEDMOREPARAMS (461)
        singleResponse resp = createSingleResponse("461", client.getSocketFdString());
        resp["<client>"] = client.getNickname();
        resp["<command>"] = message.command;
        resp["<reason>"] = ":Not enough parameters";
        responses.push_back(resp);
        return responses;
    }

    // ERR_NOORIGIN (409) not required; similar if conditions

    // Using dependency injection to receive info from Server class (prevent coupling issue)
    singleResponse resp = createSingleResponse("PONG", client.getSocketFdString());
    resp["<server>"] = serverName;
    resp["<token>"] = message.parameters[0];
    resp["<nick>"] = client.getNickname();
    responses.push_back(resp);
    return responses;
}

CommandBase* CommandPING::clone() const {
    return new CommandPING(*this);
}

