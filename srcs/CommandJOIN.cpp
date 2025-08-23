#include "CommandJOIN.hpp"

JoinCommand::JoinCommand(void) {
}

JoinCommand::~JoinCommand() {
    // Destructor logic if needed
}

void    JoinCommand::execute(Client* client, const std::vector<std::string>& params, const std::string& trailing) {

    if (params.empty)
    {
        std::// server responds with ERR_NEEDMOREPARAMS (461) */
    }
        message has no parameters

    /* if space present, split parameters into two
        // first being channel names
        // second being passwords (keys) to respective channels (if exist)
        // store inside (std::map<std::string, std::string>) */

    /* for each channel in the command (iterate through std::map)
    {
        if channel name does not exist
        {
            // server create new channel
            // server adds user to the channel (as operator)
        }
        if channel name is invalid
            // server responds with ERR_NOSUCHCHANNEL (403)
        else if channel name is valid
        {
            /// how to determine CHANLIMIT? (1) go through all channels, vs (2) keep track via count
            if client exceeds channel limit CHANLIMIT RPL_ISUPPORT,
                // server responds with ERR_TOOMANYCHANNELS (405)
            if channel password does not match
                // server responds with ERR_BADCHANNELKEY (475)
            if user is banned
                // server responds with ERR_BANNEDFROMCHAN (474)
            if (channel members) count has exceeded
                // server responds with ERR_CHANNELISFULL (471)
            if channel is set to invite-only && user is not invited
                // server responds with ERR_INVITEONLYCHAN (473)
            else
                // server returns a JOIN message
        }
    } */

    if client’s JOIN command to the server is successful
    {
        responseList response;
        singleResponse sr;

        if ()
        /// server returns the following:
        // (1) JOIN message with the client as the message <source> and
        //	the channel they have joined as the first parameter of the message
        // >> :elfoo!~elfoo@5626-2a9c-92a4-503c-675e.149.203.ip JOIN :#lobby
        ??

        // (2) The channel’s topic (with RPL_TOPIC (332)
        // >> :halcyon.il.us.dal.net 332 elfoo #lobby :miss you :P
        sr["numeric"] = "332";
        sr["client"] = client.fullname;
        //add new sr to responseList response
        //clear sr to reuse for below

        // and optionally RPL_TOPICWHOTIME (333)
        // >> :halcyon.il.us.dal.net 333 elfoo #lobby lilmoe!~lilmoe@fba-bb13-2cb4-6d98-137e.187.94.ip 1747255445
        if (!params.empty()) //	no message if the channel does not have a topic
        {
            sr["numeric"] = "333";
            sr["channel"] = params;
            sr["topic"] = trailing;
            //add new sr to responseList response
            //clear sr to reuse for below
        }
        else
        {
            sr["channel"] = trailing;
            sr["topic"] = "";
        }

        // (3) A list of users currently joined to the channel (with one or more RPL_NAMREPLY (353) numerics
        // >> :halcyon.il.us.dal.net 353 elfoo = #lobby :elfoo LDa224 Guest39645
        //	followed by a single RPL_ENDOFNAMES (366) numeric)
        // >> :halcyon.il.us.dal.net 366 elfoo #lobby :End of /NAMES list.
        //  These RPL_NAMREPLY messages sent by the server MUST include the requesting client that has just joined the channel.

        responseList = {
        {
            "<numeric>": "332",
            "<clientsToSend>": "12,22,33",  // these are client Fds seperated by commas
            "<client>": client.fullname,
            "<channel>": trailing,
            "<topic>": ?
        },
        {
            "<numeric>": "353",
            "<clientsToSend>": "12,22,33",  // these are client Fds seperated by commas
            "<client>": client.fullname,
            "<channel>": trailing,
            "<nick>": client.nickname
        },
        {
            "<numeric>": "366",
            "<clientsToSend>": "12,22,33",  // these are client Fds seperated by commas
            "<client>": client.fullname,
            "<nick>": client.nickname,
            "<user>": client.username,
            "<host>": client.hostname
        }
    }
}
