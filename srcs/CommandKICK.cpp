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

    // suggestion: check if client exists: (1) in server (401), then (2) in channel (441)
    // split target users by comma
    std::vector<std::string> targetUsers = split(message.parameters[1], ",");
    std::vector<int> targetUsersInt;
    for (std::vector<std::string>::const_iterator it = targetUsers.begin(); it != targetUsers.end(); ++it) {
        // get current channel members (currClient)
        Client* currClient = this->dataStore.getClient(*it);
        if (!currClient) {
            // ERR_NOSUCHNICK (401)
            singleResponse resp = createSingleResponse("401", client.getSocketFdString());
            resp["<client>"] = client.getClientPrefix();
            resp["<nick>"] = *it;
            resp["<reason>"] = "No such nick/channel";
            responses.push_back(resp);
            continue ;
        }
        if (!targetChannel->isMember(*currClient)) {
            // ERR_USERNOTINCHANNEL (441)
            singleResponse resp = createSingleResponse("441", client.getSocketFdString());
            resp["<client>"] = client.getClientPrefix();
            resp["<nick>"] = message.parameters[1];
            resp["<channel>"] = message.parameters[0];
            resp["<reason>"] = "They aren't on that channel";
            responses.push_back(resp);
            continue ;
        }
        targetUsersInt.push_back(currClient->getSocketFd());
    }

    if (targetUsersInt.empty())
        return responses;

    // client tries to perform a channel-affecting command on a channel
    if (!targetChannel->isMember(client)) { // client is not on that channel
        // with ERR_NOTONCHANNEL (442)
        singleResponse resp = createSingleResponse("442", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<channel>"] = message.parameters[0];
        resp["<reason>"] = "You're not on that channel";
        responses.push_back(resp);
        return responses;
    }

    if (!targetChannel->isOperator(client)) { // client is not channel operator
        // ERR_CHANOPRIVSNEEDED (482)
        singleResponse resp = createSingleResponse("482", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<channel>"] = message.parameters[0];
        resp["<reason>"] = "You're not channel operator";
        responses.push_back(resp);
        return responses;
    }

    for (std::vector<int>::const_iterator it = targetUsersInt.begin(); it != targetUsersInt.end(); ++it) {
        // if client’s KICK command to the server is successful, send acknowledgement
        Client* currMember = this->dataStore.getClient(*it);
        std::string memberFds = intSetToCSVString(targetChannel->getMembers());
        singleResponse resp = createSingleResponse("KICK", memberFds);
        resp["<nick_sender>"] = client.getClientPrefix();
        resp["<user_sender>"] = client.getUsername();
        resp["<host_sender>"] = client.getHostname();
        resp["<channel>"] = message.parameters[0];
        resp["<target_member>"] = currMember->getNickname();
        resp["<comment>"] = "";
        targetChannel->removeMember(*currMember); // automatically removes operator if operator removes himself

        //         0    1     2
        // KICK #hello user reason
        if (message.parameters.size() >= 3 && !message.parameters[2].empty()) // double confirm with second condition
            resp["<comment>"] = message.parameters[2];
        else if (!message.trailing.empty())
            resp["<comment>"] = ":" + message.trailing;
        else
            resp["<comment>"] = client.getNickname();

        responses.push_back(resp);
        continue ;
    }

    return responses;
}

CommandBase* CommandKICK::clone() const {
    return new CommandKICK(*this);
}
