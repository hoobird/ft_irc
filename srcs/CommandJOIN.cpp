#include "AllCommands.hpp"

CommandJOIN::CommandJOIN(DataStore& dataStore, const Channel& channel)
: CommandBase(), dataStore(dataStore), channel(channel) {
}

CommandJOIN::CommandJOIN(const CommandJOIN& other)
: CommandBase(), dataStore(other.dataStore) {
}

CommandJOIN::~CommandJOIN() {
    // Destructor logic if needed
}

// ERR_NOSUCHCHANNEL (403) *unsure of use case
responseList CommandJOIN::execute(Client& client, const ParsedMessage& message) {
    responseList responses;

    if (message.parameters.size() < 1) {
        // ERR_NEEDMOREPARAMS (461)
        singleResponse resp = createSingleResponse("461", client.getSocketFdString());
        resp["<client>"] = client.getClientPrefix();
        resp["<command>"] = "JOIN";
        resp["<reason>"] = "Not enough parameters";
        responses.push_back(resp);
        return responses;
    }
    // split channel name(s) and key(s) by comma (@elfoo)
    std::vector<std::string> channelNames;
    std::vector<std::string> channelKeys;

    channelNames = split(message.parameters[0], ",");
    if (message.parameters.size() > 1) {
        channelKeys = split(message.parameters[1], ",");
    }
    for (std::vector<std::string>::iterator it = channelNames.begin(); it != channelNames.end(); ++it) {
        for (); // need split to work on map to remove duplicate.
    }

    // loop through channelNames for duplicates,
    // keep the first instance found
    // remove subsequent instance of channelNames and its corresponding channelKey.

    // if there are duplicate channel names (within parameters), only the first channel name-password pair is checked

    for (int i = 0; i < channelNames.size(); ++i)  {
        Channel* findChannel = dataStore.getChannel(channelNames[i]);
        singleResponse resp;
        if (!findChannel) {
            Channel channel(channelNames[i]);
            dataStore.addChannel(&channel);
            channel.addMember(client);
        }
        else
        {
            int channelCount = dataStore.countChannelsForClient(client);
            limits limitsConfig; // MARK: TEMP (relook at scope)
            if (channelCount >= limitsConfig.CLIENT_MAX_CHANNEL) {
                resp = createSingleResponse("405", client.getSocketFdString());
                resp["<client>"] = client.getClientPrefix();
                resp["<channel>"] = findChannel->getName();
                resp["<reason>"] = "You have joined too many \channels";
                responses.push_back(resp);
                continue ;
            }
            // if channel does not need password, but password is provided, return success; password is ignored
            if (findChannel->getKey() != "" && channelKeys[i] != findChannel->getKey()) {
                // if channel is password-protected, and password is wrong, return false
                resp = createSingleResponse("475", client.getSocketFdString());
                resp["<client>"] = client.getClientPrefix();
                resp["<channel>"] = findChannel->getName();
                resp["<reason>"] = "Cannot join channel (+k)";
                responses.push_back(resp);
                continue ;
            }
            if (findChannel->getInviteMode() == true && findChannel->isMemberInvited(client) == false) {
                resp = createSingleResponse("473", client.getSocketFdString());
                resp["<client>"] = client.getClientPrefix();
                resp["<channel>"] = findChannel->getName();
                resp["<reason>"] = "Cannot join channel (+i)";
                responses.push_back(resp);
                continue ;
                // invite overrides bans (if a user is banned from an invite-only channel, if they are given invite, they can join)
            }
            if (findChannel->getMembers().size() > findChannel->getLimit()) {
                resp = createSingleResponse("471", client.getSocketFdString());
                resp["<client>"] = client.getClientPrefix();
                resp["<channel>"] = findChannel->getName();
                resp["<reason>"] = "Cannot join channel (+l)";
                responses.push_back(resp);
                continue ;
                // TODO: set arbitrary max users value; to manage in CommandMODE (both user and server)
            }
            // client joining existing channel, inform existing channel members of new member.
            std::string memberFds = intSetToCSVString(dataStore.getChannel(channelNames[i])->getMembers());
            resp = createSingleResponse("JOIN", memberFds);
            resp["<nick_sender>"] = client.getClientPrefix();
            resp["<user_sender>"] = client.getUsername();
            resp["<host_sender>"] = client.getHostname();
            resp["<channel>"] = findChannel->getName();
            continue ;
        }
        // server returns a JOIN message
        // >> :elfoo!~elfoo@5626-2a9c-92a4-503c-675e.149.203.ip JOIN :#lobby
        resp = createSingleResponse("JOIN", client.getSocketFdString());
        resp["<nick_sender>"] = client.getClientPrefix();
        resp["<user_sender>"] = client.getUsername();
        resp["<host_sender>"] = client.getHostname();
        resp["<channel>"] = findChannel->getName();
        responses.push_back(resp);
        // if client’s JOIN command to the server is successful

        //RPL_TOPIC (332)
        // >> :halcyon.il.us.dal.net 332 elfoo #lobby :miss you :P

        // (optional) RPL_TOPICWHOTIME (333)
        // >> :halcyon.il.us.dal.net 333 elfoo #lobby lilmoe!~lilmoe@fba-bb13-2cb4-6d98-137e.187.94.ip 1747255445

        // RPL_NAMREPLY (353)
        // >> :halcyon.il.us.dal.net 353 elfoo = #lobby :elfoo LDa224 Guest39645

        // RPL_ENDOFNAMES (366)
        // >> :halcyon.il.us.dal.net 366 elfoo #lobby :End of /NAMES list.
    }
    // return responses;
}

CommandBase* CommandJOIN::clone() const {
    return new CommandJOIN(*this);
}
