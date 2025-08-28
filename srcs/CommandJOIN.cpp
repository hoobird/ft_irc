#include "AllCommands.hpp"

CommandJOIN::CommandJOIN(const DataStore& dataStore)
: CommandBase(), dataStore(dataStore) {
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

    // parse channels (parameter[0]) (@elfoo)
    // TODO: Implementation to be merged later
    // NOTE: Use channelNames and channelKeys first for JOIN implementation

    // parameters have already been split by space ('2d array')
    /// if there are duplicate channel names (within parameters), only the first channel name-password pair is checked
    /// if channel does not need password, but password is provided, return success; password is ignored
    /// if channel is password-protected, and password is wrong, return false

    // dataStore is the only 'source of truth' or persists across commands
    /// if channel name is a duplicate against mapChannelKey's [key]
        /// do not overwrite existing [key]
    // if check succeeds, save channel and key respectively into mapChannelKey, ...

    // for each channel in the command (iterate through std::map) {
    //     if channel name does not exist {
    //         // server create new channel
    //         // server adds user to the channel (as operator)
    //     }
    //     else if channel name is valid
    //     {
    //         /* if client exceeds channel limit CHANLIMIT RPL_ISUPPORT,
    //             // ERR_TOOMANYCHANNELS (405)
    //             // rationale: fd exceed is caught in socket creation, setting CHANLIMIT is optional, not important */
    //             // how: (1) go through all channels, (2) keep track via count */
    //         if channel password does not match
    //             // ERR_BADCHANNELKEY (475)
    //         if user is banned
    //             // ERR_BANNEDFROMCHAN (474)
    //         if (channel members) count has exceeded
    //             // ERR_CHANNELISFULL (471)
    //             // suggested: set 'capacity' private attribute to -1 if no limit, else set limit
    //             /* TODO: set arbitrary max users value; to manage in CommandMODE (both user and server) */
    //         if channel is set to invite-only && user is not invited
    //             // ERR_INVITEONLYCHAN (473)
    //         else
    //             // server returns a JOIN message
    //             // >> :elfoo!~elfoo@5626-2a9c-92a4-503c-675e.149.203.ip JOIN :#lobby
    //     }
    // }

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
