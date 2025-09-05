#include "AllCommands.hpp"

CommandINVITE::CommandINVITE(DataStore& dataStore)
: CommandBase(), dataStore(dataStore) {
}

CommandINVITE::CommandINVITE(const CommandINVITE& other)
: CommandBase(), dataStore(other.dataStore) {
}

CommandINVITE::~CommandINVITE() {
}

responseList CommandINVITE::execute(Client& client, const ParsedMessage& message) {
    responseList responses;

    if (message.parameters.size() < 1) { // if message has no parameters
        // ERR_NEEDMOREPARAMS (461)
        singleResponse resp = createSingleResponse("461", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<command>"] = "INVITE";
        resp["<reason>"] = "Not enough parameters";
        responses.push_back(resp);
        return responses;
    }

	Channel* targetChannel = this->dataStore.getChannel(message.parameters[1]);
	if (!targetChannel) { // if channel name is invalid
		// ERR_NOSUCHCHANNEL (403)
        singleResponse resp = createSingleResponse("403", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<channel>"] = message.parameters[1];
        resp["<reason>"] = "No such channel";
        responses.push_back(resp);
        return responses;
	}

	if (!targetChannel->isMember(client)) { // client is not on that channel
        // with ERR_NOTONCHANNEL (442)
        singleResponse resp = createSingleResponse("442", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<channel>"] = targetChannel->getName();
        resp["<reason>"] = "You're not on that channel";
        responses.push_back(resp);
        return responses;
	}

    Client* targetClient = this->dataStore.getClient(message.parameters[0]);
    if (!targetClient) { // if targetClient is not in server client list
        // ERR_NOSUCHNICK (401)
        singleResponse resp = createSingleResponse("401", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<nick>"] = message.parameters[0];
        resp["<reason>"] = "No such nick/channel";
        responses.push_back(resp);
        return responses;
    }

	if (!targetChannel->isOperator(client)) { // client is not channel operator
        // ERR_CHANOPRIVSNEEDED (482)
        singleResponse resp = createSingleResponse("482", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<channel>"] = targetChannel->getName();
        resp["<reason>"] = "You're not channel operator";
        responses.push_back(resp);
        return responses;
	}

    if (targetChannel->isMember(*targetClient)) {
        // ERR_USERONCHANNEL (443)
        singleResponse resp = createSingleResponse("443", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<nick>"] = message.parameters[0];
        resp["<channel>"] = targetChannel->getName();
        resp["<reason>"] = "is already on channel";
        responses.push_back(resp);
        return responses;
    }

	/* if client’s INVITE command to the server is successful */
    // RPL_INVITING (341)
    // :serenity.fl.us.dal.net 341 elfoo elfoo_ #hello (to issuer)
    singleResponse resp = createSingleResponse("341", client.getSocketFdString());
    resp["<client>"] = client.getClientPrefix();
    resp["<nick>"] = message.parameters[0];
    resp["<channel>"] = targetChannel->getName();
    responses.push_back(resp);
    // if client’s INVITE command to the server is successful, send acknowledgement
    // :elfoo!~elfoo@5626-2a9c-92a4-503c-675e.149.203.ip INVITE elfoo_ :#hello (to target user)
    resp = createSingleResponse("INVITE", targetClient->getSocketFdString());
    resp["<nick_sender>"] = client.getClientPrefix();
    resp["<user_sender>"] = client.getUsername();
    resp["<host_sender>"] = client.getHostname();
    resp["<target_member>"] = message.parameters[0];
    resp["<channel>"] = targetChannel->getName();
    responses.push_back(resp);
    // add targetClient to invite list; use Channel class' inviteList
	targetChannel->addInvitedMember(*targetClient);
    return responses;
}

CommandBase* CommandINVITE::clone() const {
    return new CommandINVITE(*this);
}
