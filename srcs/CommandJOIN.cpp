#include "AllCommands.hpp"
#include "config.hpp" // MARK: TEMP

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

    // parameters have already been split by space ('2d array')
    /// if there are duplicate channel names (within parameters), only the first channel name-password pair is checked
    /// if channel does not need password, but password is provided, return success; password is ignored
    /// if channel is password-protected, and password is wrong, return false

    // dataStore is the only 'source of truth' or persists across commands
    /// if channel name is a duplicate against mapChannelKey's [key]
        /// do not overwrite existing [key]
    // if check succeeds, save channel and key respectively into mapChannelKey, ...

    for (int i = 0; i < channelNames.size(); ++i)  {
        Channel* findChannel = dataStore.getChannel(channelNames[i]);
        singleResponse resp;
        if (!findChannel) {
            Channel channel(channelNames[i]);
            dataStore.addChannel(&channel);
            channel.addMember(client);
            // integrate the success channel message,
        }
        else
        {
            int channelCount; // MARK: TEMP
            limits limitsConfig; // MARK: TEMP (relook at scope)
            // MARK: TEMP (Iterate through channelmap to count the number of times client is found, could refactor and make it into a helper function)
            for (DataStore::ChannelMap::const_iterator it = dataStore.getChannelsBegin(); it != dataStore.getChannelsEnd(); ++it) {
                if (it->second->isMember(client))
                    channelCount++;
            }
            // MARK: TEMP
            if (channelCount >= limitsConfig.CLIENT_MAX_CHANNEL) {
                resp = createSingleResponse("405", client.getSocketFdString());
                resp["<client>"] = client.getClientPrefix();
                resp["<channel>"] = findChannel->getName();
                resp["<reason>"] = "You have joined too many \channels";
                responses.push_back(resp);
                continue ;
                // ERR_TOOMANYCHANNELS (405)
                // rationale: fd exceed is caught in socket creation, setting CHANLIMIT is optional, not important
                // how: (1) go through all channels, (2) keep track via count */
            }
            if (channelKeys[i] != findChannel->getKey()) {
                // ERR_BADCHANNELKEY (475)
                resp = createSingleResponse("475", client.getSocketFdString());
                resp["<client>"] = client.getClientPrefix();
                resp["<channel>"] = findChannel->getName();
                resp["<reason>"] = "Cannot join channel (+k)";
                responses.push_back(resp);
                continue ;
            }
            if (/* channel is set to invite-only && user is not invited */) {
                // ERR_INVITEONLYCHAN (473)
                resp = createSingleResponse("473", client.getSocketFdString());
                resp["<client>"] = client.getClientPrefix();
                resp["<channel>"] = findChannel->getName();
                resp["<reason>"] = "Cannot join channel (+i)";
                responses.push_back(resp);
                continue ;
                // invite overrides bans (if a user is banned from an invite-only channel, if they are given invite, they can join)
            }
            if (/* user is banned */) {
                // ERR_BANNEDFROMCHAN (474)
                resp = createSingleResponse("474", client.getSocketFdString());
                resp["<client>"] = client.getClientPrefix();
                resp["<channel>"] = findChannel->getName();
                resp["<reason>"] = "Cannot join channel (+b)";
                responses.push_back(resp);
                continue ;
            }
            if (findChannel->getMembers().size() > findChannel->getLimit())
                // ERR_CHANNELISFULL (471)
                resp = createSingleResponse("471", client.getSocketFdString());
                resp["<client>"] = client.getClientPrefix();
                resp["<channel>"] = findChannel->getName();
                resp["<reason>"] = "Cannot join channel (+l)";
                responses.push_back(resp);
                continue ;
                // suggested: set 'capacity' private attribute to -1 if no limit, else set limit
                // TODO: set arbitrary max users value; to manage in CommandMODE (both user and server)
            // client joining existing channel, inform existing channel members of new member.
            std::string memberFds = intSetToCSVString(dataStore.getChannel(channelNames[i])->getMembers());
            resp = createSingleResponse("JOIN", memberFds);
            resp["<nick_sender>"] = client.getClientPrefix();
            resp["<user_sender>"] = client.getUsername();
            resp["<host_sender>"] = client.getHostname();
            resp["<channel>"] = findChannel->getName();
        }
        // server returns a JOIN message
        // >> :elfoo!~elfoo@5626-2a9c-92a4-503c-675e.149.203.ip JOIN :#lobby
        resp = createSingleResponse("JOIN", client.getSocketFdString());
        resp["<nick_sender>"] = client.getClientPrefix();
        resp["<user_sender>"] = client.getUsername();
        resp["<host_sender>"] = client.getHostname();
        resp["<channel>"] = findChannel->getName();
    }

    // if client’s JOIN command to the server is successful
    // {
    //     responseList response;
    //     singleResponse resp;

    //     // RPL_TOPIC (332)
    //     // >> :halcyon.il.us.dal.net 332 elfoo #lobby :miss you :P

    //     // (optional) RPL_TOPICWHOTIME (333)
    //     // >> :halcyon.il.us.dal.net 333 elfoo #lobby lilmoe!~lilmoe@fba-bb13-2cb4-6d98-137e.187.94.ip 1747255445

    //     // RPL_NAMREPLY (353)
    //     // >> :halcyon.il.us.dal.net 353 elfoo = #lobby :elfoo LDa224 Guest39645

    //     // RPL_ENDOFNAMES (366)
    //     // >> :halcyon.il.us.dal.net 366 elfoo #lobby :End of /NAMES list.
    // }
}

CommandBase* CommandJOIN::clone() const {
    return new CommandJOIN(*this);
}
