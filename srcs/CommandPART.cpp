#include "AllCommands.hpp"

CommandPART::CommandPART(DataStore &dataStore): CommandBase(), dataStore(dataStore)
{
}

CommandPART::CommandPART(const CommandPART &other): CommandBase(), dataStore(other.dataStore)
{
}
CommandPART::~CommandPART()
{
}


// 461 ERR_NEEDMOREPARAMS
// 403 ERR_NOSUCHCHANNEL
// 442 ERR_NOTONCHANNEL
responseList CommandPART::execute(Client& client, const ParsedMessage& message)
{
    responseList responses;
    const std::string clientFdStr = client.getSocketFdString();
    const std::string clientNick = client.getClientPrefix();

    if (message.parameters[0].empty()) {
        singleResponse resp = createSingleResponse("461", clientFdStr);
        resp["<client>"] = client.getClientPrefix();
        resp["<command>"] = "PART";
        resp["<reason>"] = "Not enough parameters";
        responses.push_back(resp);
        return responses;
    }

    std::vector<std::string> channelNames;
    channelNames = split(message.parameters[0], ",");
    for (size_t i = 0; i < channelNames.size(); ++i) {
        Channel* findChannel = dataStore.getChannel(channelNames[i]);
        // channel does not exist
        if (!findChannel) {
            singleResponse resp = createSingleResponse("403", clientFdStr);
            resp["<client>"] = clientNick;
            resp["<channel>"] = channelNames[i];
            resp["<reason>"] = "No such channel";
            responses.push_back(resp);
            continue ;
        }
        // if client is not a member of channel:
        if (findChannel->isMember(client) == false) {
            singleResponse resp = createSingleResponse("442", clientFdStr);
            resp["<client>"] = clientNick;
            resp["<channel>"] = channelNames[i];
            resp["<reason>"] = "You're not on that channel";
            responses.push_back(resp);
            continue ;
        }
        // notify all members of the channel about the PART (with reason)
        std::string memberFds = intSetToCSVString(findChannel->getMembers());
        singleResponse resp = createSingleResponse("PART", memberFds); // everybody receives this, client included.
        resp["<nick_sender>"] = clientNick;
        resp["<user_sender>"] = client.getUsername();
        resp["<host_sender>"] = client.getHostname();
        resp["<channel>"] = channelNames[i];
        resp["<reason>"] = "";

        // if parameter[1] exists, single-word reason provided
        if (message.parameters.size() > 1) {
            resp["<reason>"] = message.parameters[1];
        }
        // trailing present = reason provided
        if (!message.trailing.empty()) {
            resp["<reason>"] = message.trailing;
        }
        responses.push_back(resp);

        findChannel->removeMember(client);
        if (findChannel->getMembers().empty() == true) {
            dataStore.removeChannel(channelNames[i]);
        }
    }
    return responses;
}

CommandBase* CommandPART::clone() const
{
    return new CommandPART(*this);
}
