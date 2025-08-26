#include "AllCommands.hpp"

CommandMODE::CommandMODE(DataStore const & dataStore)
    : dataStore(dataStore)
{
}

CommandMODE::CommandMODE(const CommandMODE &other): CommandBase(), dataStore(other.dataStore)
{
}

CommandMODE::~CommandMODE()
{
}

responseList CommandMODE::execute(Client& client, const ParsedMessage& message) {
    responseList responses;

    // 461 ERR_NEEDMOREPARAMS
    if (message.parameters.size() < 1) {
        singleResponse resp = createSingleResponse("461", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<command>"] = "MODE";
        resp["<reason>"] = "Not enough parameters";
        responses.push_back(resp);
        return responses;
    }
    // we dont handle client also
    // 501 ERR_UMODEUNKNOWNFLAG
    if (message.parameters[0][0] != '#') {
        singleResponse resp = createSingleResponse("501", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<reason>"] = "This server does not support user modes";
        responses.push_back(resp);
        return responses;
    }

    // PLEASE IMPLEMENT CHANNEL MODE HERE
    return responses;
}

CommandBase* CommandMODE::clone() const {
    return new CommandMODE(*this);
}
