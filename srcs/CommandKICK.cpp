#include "AllCommands.hpp"

CommandKICK::CommandKICK(DataStore& dataStore)
: CommandBase(), dataStore(dataStore) {
}

CommandKICK::CommandKICK(const CommandKICK& other)
: CommandBase(), dataStore(other.dataStore) {
}

CommandKICK::~CommandKICK() {
}

/* Context: command can be used to request the forced removal of a user from a channel.
It causes the <user> to be removed from the <channel> by force. */
responseList CommandKICK::execute(Client& client, const ParsedMessage& message) {
    responseList responses;

    if (message.parameters.size() < 1) { // if message has no parameters
        // ERR_NEEDMOREPARAMS (461)
        singleResponse resp = createSingleResponse("461", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<command>"] = "KICK";
        resp["<reason>"] = "Not enough parameters";
        responses.push_back(resp);
        return responses;
    }

    Channel* targetChannel = this->dataStore.getChannel(message.parameters[0]);
    if (!targetChannel) { // if channel name is invalid
        // ERR_NOSUCHCHANNEL (403)
        singleResponse resp = createSingleResponse("403", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<channel>"] = message.parameters[0];
        resp["<reason>"] = "No such channel";
        responses.push_back(resp);
        return responses;
    }

    std::set<int> operatorList = dataStore.getChannel(targetChannel->getName())->getOperators();
    std::set<int>::iterator it = operatorList.find(client.getSocketFd());
    if (it == operatorList.end()) { // client is not channel operator
        // ERR_CHANOPRIVSNEEDED (482)
        singleResponse resp = createSingleResponse("482", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<channel>"] = message.parameters[0];
        resp["<reason>"] = "You're not channel operator";
        responses.push_back(resp);
        return responses;
    }

    // TODO: consider scenario where operator kicks multiple members
    // client tries to perform a channel+nick affecting command)
    std::set<int> memberList = dataStore.getChannel(targetChannel->getName())->getMembers();
    std::set<int>::iterator it = memberList.find(client.getSocketFd());
    if (it == memberList.end()) { // client is not joined to the channel
        // ERR_USERNOTINCHANNEL (441)
        singleResponse resp = createSingleResponse("441", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<nick>"] = *it;
        resp["<channel>"] = message.parameters[0];
        resp["<reason>"] = "They aren't on that channel";
        responses.push_back(resp);
        return responses;
    }

    // // client tries to perform a channel-affecting command on a channel)
    // if (?) { // client is not on that channel
    //     // with ERR_NOTONCHANNEL (442)
    //     singleResponse resp = createSingleResponse("442", client.getSocketFdString());
    //     resp["<client>"] = client.getClientPrefix();
    //     resp["<channel>"] = (?);
    //     resp["<reason>"] = "You're not on that channel";
    //     responses.push_back(resp);
    //     return responses;
    // }

    /* if client’s KICK command to the server is successful, send acknowledgement */
    // remember to removeMember()!!

    /* (optional) This message may be sent from a server to a client to notify the client that someone has been removed from a channel.
    In this case, the message <source> will be the client who sent the kick,
        and <channel> will be the channel which the target client has been removed from. */

    return responses;
}

CommandBase* CommandKICK::clone() const {
    return new CommandKICK(*this);
}
