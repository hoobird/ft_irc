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
        singleResponse res = createSingleResponse("411", client.getSocketFdString());
        res["<client>"] = client.getClientPrefix();
        res["<reason>"] = "No recipient given (PRIVMSG)";
        responses.push_back(res);
        return responses;
    }

    if (!message.parameters[0].empty()) {
        if (message.parameters[0][0] != '#') {
            // check if client
            Client* recipient = dataStore.getClient(message.parameters[0]);
            if (!recipient) {
                singleResponse res = createSingleResponse("401", client.getSocketFdString());
                res["<client>"] = client.getClientPrefix();
                res["<nick>"] = message.parameters[0];
                res["<reason>"] = "No such nick/channel";
                responses.push_back(res);
                return responses;
            }
            if (message.trailing.empty()) {
                singleResponse res = createSingleResponse("412", client.getSocketFdString());
                res["<client>"] = client.getClientPrefix();
                res["<reason>"] = "No text to send";
                responses.push_back(res);
                return responses;
            }
            singleResponse res = createSingleResponse("PRIVMSG", recipient->getSocketFdString());
            res["<nick_sender>"] = client.getNickname();
            res["<user_sender>"] = client.getUsername();
            res["<host_sender>"] = client.getHostname();
            res["<msg_receiver>"] = recipient->getNickname();
            res["<msg>"] = message.trailing;
            responses.push_back(res);
        }
        // else {
        //     // check if channel
        //     Channel* targetChannel;
        // }
    }
    return responses;
}

CommandBase* CommandPRIVMSG::clone() const
{
    return new CommandPRIVMSG(*this);
}