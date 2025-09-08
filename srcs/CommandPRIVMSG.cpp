#include "AllCommands.hpp"

CommandPRIVMSG::CommandPRIVMSG(const DataStore& dataStore): CommandBase(), dataStore(dataStore)
{
}

CommandPRIVMSG::CommandPRIVMSG(const CommandPRIVMSG& other): CommandBase(), dataStore(other.dataStore)
{
}

CommandPRIVMSG::~CommandPRIVMSG()
{
}

// ERR_NORECIPIENT (411)
// ERR_NOSUCHNICK (401)
// ERR_NOTEXTTOSEND (412)
// ERR_TOOMANYTARGETS (407) *if we want to handle later
responseList CommandPRIVMSG::execute(Client& client, const ParsedMessage& message)
{
    responseList responses;

    if (message.parameters.size() < 1) {
        singleResponse resp = createSingleResponse("411", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<reason>"] = "No recipient given (PRIVMSG)";
        responses.push_back(resp);
        return responses;
    }
    if (!message.parameters[0].empty()) {
        std::vector<std::string> recipients;
        recipients = split(message.parameters[0], ",");
        std::vector<std::string>::const_iterator it;
        for (it = recipients.begin(); it != recipients.end(); ++it) {
            if (it->at(0) != '#') {
                responseList clientResponses = handleClientRecipient(client, *it, message);
                responses.insert(responses.end(), clientResponses.begin(), clientResponses.end());
            } else {
                responseList channelResponses = handleChannelRecipient(client, *it, message);
                responses.insert(responses.end(), channelResponses.begin(), channelResponses.end());
            }
        }
    }
    return responses;
}

CommandBase* CommandPRIVMSG::clone() const
{
    return new CommandPRIVMSG(*this);
}

responseList CommandPRIVMSG::handleClientRecipient(Client& sender, const std::string& recipientNick, const ParsedMessage& message) {
    responseList responses;
    Client* recipient = dataStore.getClient(recipientNick);
    if (!recipient) {
        singleResponse resp = createSingleResponse("401", sender.getSocketFdString());
        resp["<client>"] = sender.getClientPrefix();
        resp["<nick>"] = recipientNick;
        resp["<reason>"] = "No such nick/channel";
        responses.push_back(resp);
        return responses;
    }
    if (message.trailing.empty()) {
        singleResponse resp = createSingleResponse("412", sender.getSocketFdString());
        resp["<client>"] = sender.getClientPrefix();
        resp["<reason>"] = "No text to send";
        responses.push_back(resp);
        return responses;
    }
    if (CTCPHandler::isCTCPCommand(message)) {
        CTCPHandler ctcpHandler;
        responseList ctcpResponses = ctcpHandler.handleCTCPCommand(sender, *recipient, message);
        responses.insert(responses.end(), ctcpResponses.begin(), ctcpResponses.end());
        return responses;
    }
    singleResponse resp = createSingleResponse("PRIVMSG", recipient->getSocketFdString());
    resp["<nick_sender>"] = sender.getNickname();
    resp["<user_sender>"] = sender.getUsername();
    resp["<host_sender>"] = sender.getHostname();
    resp["<msg_receiver>"] = recipient->getNickname();
    resp["<msg>"] = message.trailing;
    responses.push_back(resp);
    return responses;
}

// Private helper for channel recipient
responseList CommandPRIVMSG::handleChannelRecipient(Client& sender, const std::string& channelName, const ParsedMessage& message) {
    responseList responses;
    Channel* targetChannel = dataStore.getChannel(channelName);
    if (!targetChannel) {
        singleResponse resp = createSingleResponse("401", sender.getSocketFdString());
        resp["<client>"] = sender.getClientPrefix();
        resp["<nick>"] = channelName;
        resp["<reason>"] = "No such nick/channel";
        responses.push_back(resp);
        return responses;
    }
    if (message.trailing.empty()) {
        singleResponse resp = createSingleResponse("412", sender.getSocketFdString());
        resp["<client>"] = sender.getClientPrefix();
        resp["<reason>"] = "No text to send";
        responses.push_back(resp);
        return responses;
    }
    std::set<int> memberFd = targetChannel->getMembers();
    std::string memberFdStr = this->intSetToCSVString(memberFd);
    singleResponse resp = createSingleResponse("PRIVMSG", memberFdStr);
    resp["<nick_sender>"] = sender.getNickname();
    resp["<user_sender>"] = sender.getUsername();
    resp["<host_sender>"] = sender.getHostname();
    resp["<msg_receiver>"] = targetChannel->getName();
    resp["<msg>"] = message.trailing;
    responses.push_back(resp);
    return responses;
}
