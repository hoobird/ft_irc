#include "AllCommands.hpp"

CommandUSER::CommandUSER(DataStore const & dataStore)
    : dataStore(dataStore)
{
}

CommandUSER::CommandUSER(const CommandUSER &other): CommandBase(), dataStore(other.dataStore)
{
    (void)other;
}

CommandUSER::~CommandUSER()
{
}

responseList CommandUSER::execute(Client &client, const ParsedMessage &message)
{
    // Example: USER guest 0 * Bob  (4 paramseters)
    // OR
    // Example: USER guest 0 * :Bob Boulder (3 params + trailing)
    // ERR_NEEDMOREPARAMS 461
    responseList responses;

    // is already registered
    if (client.isRegistered()) {
        singleResponse resp = createSingleResponse("462", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<reason>"] = ":You may not reregister";
        responses.push_back(resp);
        return responses;
    }

    // check parameters
    bool is4param = message.parameters.size() >= 4;
    bool is3paramPlusTrailing = message.parameters.size() >= 3 && !message.trailing.empty();
    if (!is4param && !is3paramPlusTrailing) {
        singleResponse resp = createSingleResponse("461", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<command>"] = "USER";
        resp["<reason>"] = ":Not enough parameters";
        responses.push_back(resp);
        return responses;
    }

    // set username and realname
    client.setUsername(message.parameters[0]);
    if (is4param) {
        client.setRealname(message.parameters[3]);
    } else { // is3paramPlusTrailing
        client.setRealname(message.trailing);
    }

    // because of first if check, we know client is not registered yet
    if (!client.isReadyToRegister())
        return responses; // still not ready to register, no response needed (responses is still empty here)
    client.setRegistered();
    return createWelcomeResponse(client);
}

CommandBase *CommandUSER::clone() const
{
    return new CommandUSER(*this);
}
