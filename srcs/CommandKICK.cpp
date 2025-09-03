#include "AllCommands.hpp"
#include <algorithm>

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

    std::set<int> operatorList = targetChannel->getOperators();
    std::set<int>::iterator OperatorIt = operatorList.find(client.getSocketFd()); // find method in std::set
    if (OperatorIt == operatorList.end()) { // client is not channel operator
        // ERR_CHANOPRIVSNEEDED (482)
        singleResponse resp = createSingleResponse("482", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<channel>"] = message.parameters[0];
        resp["<reason>"] = "You're not channel operator";
        responses.push_back(resp);
        return responses;
    }

    // client tries to perform a channel+nick affecting command
    std::string members = intSetToCSVString(targetChannel->getMembers());
    std::vector<std::string> memberList = split(members, ",");
    // >> REWORK (START) 441 error numeric
    std::vector<std::string> SuggestedMemberList = split(message.parameters[1], ",");
    for (as we go through the vector via iterator, we obtain the client pointer) {
        Client* targetClient = this->dataStore.getClient(message.parameters[1]);
        if (memberIt == memberList.end()) { // client is not joined to the channel
            // ERR_USERNOTINCHANNEL (441)
            singleResponse resp = createSingleResponse("441", client.getSocketFdString());
            resp["<client>"] = client.getClientPrefix();
            resp["<nick>"] = message.parameters[1];
            resp["<channel>"] = message.parameters[0];
            resp["<reason>"] = "They aren't on that channel";
            responses.push_back(resp);
            return responses;
        }
    }
    /* std::vector<std::string>::const_iterator memberIt = std::find(memberList.begin(), memberList.end(), message.parameters[1]); // find function for containers
    std::cout << "message.parameters[1]: " << message.parameters[1] << std::endl;
    for (std::vector<std::string>::const_iterator it = memberList.begin(); it != memberList.end(); ++it) {
        std::cout << "Member: " << *it << std::endl;
    }
    if (memberIt == memberList.end()) { // client is not joined to the channel
        // ERR_USERNOTINCHANNEL (441)
        singleResponse resp = createSingleResponse("441", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<nick>"] = message.parameters[1];
        resp["<channel>"] = message.parameters[0];
        resp["<reason>"] = "They aren't on that channel";
        responses.push_back(resp);
        return responses;
    } */
    // REWORK (END) 441 error numeric

    // client tries to perform a channel-affecting command on a channel
    memberIt = std::find(memberList.begin(), memberList.end(), client.getSocketFdString());
    if (memberIt == memberList.end()) { // client is not on that channel
        // with ERR_NOTONCHANNEL (442)
        singleResponse resp = createSingleResponse("442", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<channel>"] = message.parameters[0];
        resp["<reason>"] = "You're not on that channel";
        responses.push_back(resp);
        return responses;
    }

    /* if client’s KICK command to the server is successful, send acknowledgement */
    // remember to removeMember()!!
    targetChannel->removeMember(client); // automatically removes operator if operator removes himself
    singleResponse resp = createSingleResponse("KICK", client.getSocketFdString());
    resp["<channel>"] = message.parameters[0];
    resp["<user_sender>"] = client.getUsername();
    resp["<comment>"] = message.trailing;
    responses.push_back(resp);

    /* (optional) This message may be sent from a server to a client to notify the client that someone has been removed from a channel.
    In this case, the message <source> will be the client who sent the kick,
        and <channel> will be the channel which the target client has been removed from. */

    return responses;
}

CommandBase* CommandKICK::clone() const {
    return new CommandKICK(*this);
}
