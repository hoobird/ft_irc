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
        if (message.parameters[0][0] != '#') {
            // check if client
            Client* recipient = dataStore.getClient(message.parameters[0]);
            if (!recipient) {
                singleResponse resp = createSingleResponse("401", client.getSocketFdString());
                resp["<client>"] = client.getClientPrefix();
                resp["<nick>"] = message.parameters[0];
                resp["<reason>"] = "No such nick/channel";
                responses.push_back(resp);
                return responses;
            }
            if (message.trailing.empty()) {
                singleResponse resp = createSingleResponse("412", client.getSocketFdString());
                resp["<client>"] = client.getClientPrefix();
                resp["<reason>"] = "No text to send";
                responses.push_back(resp);
                return responses;
            }
            singleResponse resp = createSingleResponse("PRIVMSG", recipient->getSocketFdString());
            resp["<nick_sender>"] = client.getNickname();
            resp["<user_sender>"] = client.getUsername();
            resp["<host_sender>"] = client.getHostname();
            resp["<msg_receiver>"] = recipient->getNickname();
            resp["<msg>"] = message.trailing;
            responses.push_back(resp);
        }
        else {
            // if it's to channel
            Channel* targetChannel = this->dataStore.getChannel(message.parameters[0]);

            if (!targetChannel) {
                singleResponse resp = createSingleResponse("401", client.getSocketFdString());
                resp["<client>"] = client.getClientPrefix();
                resp["<nick>"] = message.parameters[0];
                resp["<reason>"] = "No such nick/channel";
                responses.push_back(resp);
                return responses;
            }
            if (message.trailing.empty()) {
                singleResponse resp = createSingleResponse("412", client.getSocketFdString());
                resp["<client>"] = client.getClientPrefix();
                resp["<reason>"] = "No text to send";
                responses.push_back(resp);
                return responses;
            }
            std::set<int> memberFd = targetChannel->getMembers();
            std::string memberFdStr = this->intSetToCSVString(memberFd);
            singleResponse resp = createSingleResponse("PRIVMSG", memberFdStr);
            resp["<nick_sender>"] = client.getNickname();
            resp["<user_sender>"] = client.getUsername();
            resp["<host_sender>"] = client.getHostname();
            resp["<msg_receiver>"] = targetChannel->getName();
            resp["<msg>"] = message.trailing;
            responses.push_back(resp);
        }
    }
    return responses;
}

CommandBase* CommandPRIVMSG::clone() const
{
    return new CommandPRIVMSG(*this);
}
