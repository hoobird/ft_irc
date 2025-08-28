#include "AllCommands.hpp"

CommandPASS::CommandPASS(const AuthService& authService, std::string serverName)
    : authService(authService), serverName(serverName)
{
}

CommandPASS::CommandPASS(const CommandPASS &other): CommandBase(), authService(other.authService), serverName(other.serverName)
{
    (void) other;
}

CommandPASS::~CommandPASS()
{
}

// ERR_ALREADYREGISTERED 462
// ERR_NEEDMOREPARAMS 461
// ERR_PASSWDMISMATCH 464
responseList CommandPASS::execute(Client &client, const ParsedMessage &message)
{
    responseList responses;
    if (client.isRegistered())
    {
        singleResponse resp = createSingleResponse("462", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<reason>"] = ":You may not reregister";
        responses.push_back(resp);
        return responses;
    }
    if (message.parameters.size() < 1)
    {
        singleResponse resp = createSingleResponse("461", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<reason>"] = ":Not enough parameters";
        responses.push_back(resp);
        return responses;
    }
    const std::string& password = message.parameters[0];
    if (!authService.validatePassword(password))
    {
        singleResponse resp = createSingleResponse("464", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<reason>"] = ":Password incorrect";
        responses.push_back(resp);
        return responses;
    }

    // because of first if check, we know client is not registered yet
    client.setPasswordValidated();
    if (!client.isReadyToRegister())
        return responses; // still not ready to register, no response needed (responses is still empty here
    client.setRegistered();
    return createWelcomeResponse(client, serverName);
}

CommandBase *CommandPASS::clone() const
{
    return new CommandPASS(*this);
}
