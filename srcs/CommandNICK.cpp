#include "AllCommands.hpp"

CommandNICK::CommandNICK(const DataStore &dataStore, std::string serverName)
    : dataStore(dataStore), serverName(serverName)
{
}

CommandNICK::CommandNICK(const CommandNICK &other): CommandBase(), dataStore(other.dataStore), serverName(other.serverName)
{
    (void)other; // nothing to copy
}

CommandNICK::~CommandNICK()
{
}

responseList CommandNICK::execute(Client &client, const ParsedMessage &message)
{
    // ERR_NONICKNAMEGIVEN 431
    // ERR_ERRONEUSNICKNAME 432
    // ERR_NICKNAMEINUSE 433
    responseList responses;
    if (message.parameters.size() < 1)
    {
        singleResponse resp = createSingleResponse("431", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<reason>"] = "No nickname given";
        responses.push_back(resp);
        return responses;
    }
    const std::string &newNick = message.parameters[0];
    if (!isValidNickname(newNick))
    {
        singleResponse resp = createSingleResponse("432", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<nick>"] = newNick;
        resp["<reason>"] = "Erroneous nickname";
        responses.push_back(resp);
        return responses;
    }
    for (DataStore::ClientMap::const_iterator it = dataStore.getClientsBegin(); it != dataStore.getClientsEnd(); ++it)
    {
        Client& exisitingClient = *(it->second);
        if (exisitingClient.getNickname() == newNick)
        {
            singleResponse resp = createSingleResponse("433", client.getSocketFdString());
            resp["<client>"] = client.getClientPrefix();
            resp["<nick>"] = newNick;
            resp["<reason>"] = "Nickname is already in use";
            responses.push_back(resp);
            return responses;
        }
    }
    std::string oldNick = client.getNickname().empty() ? "*" : client.getNickname();
    client.setNickname(newNick);

    // 1) If haventt registered yet
    //   a) If not ready to register yet, no response needed
    //   b) If ready to register now, set registered and send welcome messages
    // 2) If already registered, broadcast to other clients
    if (!client.isRegistered()){
        if (!client.isReadyToRegister())
            return responses; // still not ready to register, no response needed (responses is still empty here)
        // Client is now ready to register
        client.setRegistered();
        // lets just send successful nick change first in order to update irssi nickname display
        singleResponse selfResponse = createSingleResponse("NICK", client.getSocketFdString());
        selfResponse["<oldnick>"] = newNick;
        selfResponse["<user>"] = client.getUsername();
        selfResponse["<host>"] = client.getHostname();
        selfResponse["<new_nick>"] = newNick;
        responseList welcome = createWelcomeResponse(client, serverName);
        responses.insert(responses.end(), welcome.begin(), welcome.end());
        responses.push_back(selfResponse);
        return responses;
    }
    std::cout << "Client " << client.getSocketFd() << " changed nick from " << oldNick << " to " << newNick << std::endl;
    // Tell User about their own nick change
    // and also tell other users in same channel about the nick change
    std::set<int> clientsToNotify; // channel buddies
    clientsToNotify.insert(client.getSocketFd()); // include self
    for (DataStore::ChannelMap::const_iterator chanIt = dataStore.getChannelsBegin(); chanIt != dataStore.getChannelsEnd(); ++chanIt)
    {
        Channel& channel = *(chanIt->second);
        if (channel.isMember(client))
        {
            const std::set<int>& members = channel.getMembers();
            clientsToNotify.insert(members.begin(), members.end());
        }
    }
    singleResponse selfResponse = createSingleResponse("NICK", intSetToCSVString(clientsToNotify));
    selfResponse["<oldnick>"] = oldNick;
    selfResponse["<user>"] = client.getUsername();
    selfResponse["<host>"] = client.getHostname();
    selfResponse["<new_nick>"] = newNick;
    responses.push_back(selfResponse);

    return responses;
}

CommandBase *CommandNICK::clone() const
{
    return new CommandNICK(*this);
}

bool CommandNICK::isValidNickname(std::string nick)
{
    if (nick.empty())
        return false;
    if (!isalpha(nick[0]))
        return false; // nicknames cannot start with channel prefix
    if (nick.empty())
        return false;
    for (size_t i = 0; i < nick.size(); ++i)
    {
        char c = nick[i];
        if (!(std::isalnum(c) || c == '[' || c == ']' || c == '{' || c == '}'
            || c == '\\' || c == '|' || c == '-' || c == '^' || c == '`' || c == '_'))
            return false; // invalid character found
    }
    return true;
}
