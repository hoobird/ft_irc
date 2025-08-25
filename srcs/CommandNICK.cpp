#include "AllCommands.hpp"

CommandNICK::CommandNICK(const DataStore &dataStore)
    : dataStore(dataStore)
{
}

CommandNICK::CommandNICK(const CommandNICK &other): CommandBase(), dataStore(other.dataStore)
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
        resp["<reason>"] = ":No nickname given";
        responses.push_back(resp);
        return responses;
    }
    const std::string &newNick = message.parameters[0];
    if (!isValidNickname(newNick))
    {
        singleResponse resp = createSingleResponse("432", client.getSocketFdString());
        resp["<nick>"] = newNick;
        resp["<reason>"] = ":Erroneous nickname";
        responses.push_back(resp);
        return responses;
    }
    for (DataStore::ClientMap::const_iterator it = dataStore.getClientsBegin(); it != dataStore.getClientsEnd(); ++it)
    {
        Client& exisitingClient = *(it->second);
        if (exisitingClient.getNickname() == newNick)
        {
            singleResponse resp = createSingleResponse("433", client.getSocketFdString());
            resp["<nick>"] = newNick;
            resp["<reason>"] = ":Nickname is already in use";
            responses.push_back(resp);
            return responses;
        }
    }
    std::string oldNick = client.getNickname().empty() ? "" : client.getNickname();
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
        return createWelcomeResponse(client);
    }
    
    // Tell User about their own nick change
    // and also tell other users in same channel about the nick change
    std::set<int> clientsToNotify; // channel buddies
    for (DataStore::ChannelMap::const_iterator chanIt = dataStore.getChannelsBegin(); chanIt != dataStore.getChannelsEnd(); ++chanIt)
    {
        Channel& channel = *(chanIt->second);
        if (channel.isMember(client))
        {
            const std::set<int>& members = channel.getMembers();
            clientsToNotify.insert(members.begin(), members.end());
        }
    }
    singleResponse selfResponse = createSingleResponse("NICK", client.getSocketFdString());
    selfResponse["<oldnick>"] = oldNick;
    selfResponse["<user>"] = client.getUsername();
    selfResponse["<host>"] = client.getHostname();
    selfResponse["<new_nick>"] = newNick;

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
    if (nick[0] == '#' || nick[0] == ':')
        return false; // nicknames cannot start with channel prefix
    if (nick.empty())
        return false;
    for (size_t i = 0; i < nick.size(); ++i)
    {
        char c = nick[i];
        if (!(std::isalnum(c) || c == '[' || c == ']' || c == '{' || c == '}' || c == '\\' || c == '|'))
            return false; // invalid character found
    }
    return true;
}
