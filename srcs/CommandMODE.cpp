#include "AllCommands.hpp"

CommandMODE::CommandMODE(DataStore const & dataStore): CommandBase(), dataStore(dataStore)
{
}

CommandMODE::CommandMODE(const CommandMODE &other): CommandBase(), dataStore(other.dataStore)
{
}

CommandMODE::~CommandMODE()
{
}

// 461 ERR_NEEDMOREPARAMS (done)
// 501 ERR_UMODEUNKNOWNFLAG (done)
// 403 ERR_NOSUCHCHANNEL (done)
// 442 ERR_NOTONCHANNEL (Returned when a client tries to perform a channel-affecting command on a channel which the client isn’t a part of.)
// 401 ERR_NOSUCHNICK (Indicates that no client can be found for the supplied nickname. The text used in the last param of this message may vary.)
// 324 RPL_CHANNELMODEIS (sent to a client to inform them of the currently active modes of a channel)
// 472 ERR_UNKNOWNMODE (Indicates that a mode character used by a client is not recognized by the server. The text used in the last param of this message may vary.)
// 482 ERR_CHANOPRIVSNEEDED (Indicates that a command failed because the client does not have the appropriate channel privileges. This numeric can apply for different prefixes such as halfop, operator, etc. The text used in the last param of this message may vary.)
// 467 ERR_KEYSET (Implemented for classic RFC compliant server, where if a key is set already, it needs to be removed first, dalnet simply overwrites with +k if pass was set alr)

// Channel Mode Parameters: <channel> {[+|-]|o|i|t|k|l} [<limit>] [<user>] [<ban mask>]

// need to do basic parsing (if + or -, then followed by whatever mode)
responseList CommandMODE::execute(Client& client, const ParsedMessage& message) {
    responseList responses;
    const std::string clientNick = client.getClientPrefix();
    const std::string clientFdStr = client.getSocketFdString();

    // 461 ERR_NEEDMOREPARAMS
    if (message.parameters.size() < 1) {
        singleResponse resp = createSingleResponse("461", clientFdStr);
        resp["<client>"] = clientNick;
        resp["<command>"] = "MODE";
        resp["<reason>"] = "Not enough parameters";
        responses.push_back(resp);
        return responses;
    }
    // we dont handle client also
    // 501 ERR_UMODEUNKNOWNFLAG
    if (message.parameters[0][0] != '#') {
        singleResponse resp = createSingleResponse("501", clientFdStr);
        resp["<client>"] = clientNick;
        resp["<reason>"] = "This server does not support user modes";
        responses.push_back(resp);
        return responses;
    }
    const std::string channelName = message.parameters[0];
    Channel* targetChannel = dataStore.getChannel(channelName);
    if (!targetChannel) {
        // ERR_NOSUCHCHANNEL
        singleResponse resp = createSingleResponse("403", clientFdStr);
        resp["<client>"] = clientNick;
        resp["<channel>"] = channelName;
        resp["<reason>"] = "No such channel";
        responses.push_back(resp);
        return responses;
    }
    // WIP (JUST A FORMAT)
    // if ("-")
        // loop through string?
        // if (string[i] == 'i')
            // remove invite-only status on channel
        // if (string[i] == 't')
            // remove topic restriction on channel
        // if (string[i] == 'k')
            // remove password on channel
        // if (string[i] == 'o')
            // remove channel operator privileges
        // if (string[i] == 'l')
            // remove user limit to channel
    // if ("+")
        // loop through string?
        // if (string[i] == 'i')
            // set invite-only status on channel
        // if (string[i] == 't')
            // set topic restriction on channel
        // if (string[i] == 'k')
            // set password on channel
        // if (string[i] == 'o')
            // give channel operator privileges
        // if (string[i] == 'l')
            // set user limit to channel

    return responses;
}

CommandBase* CommandMODE::clone() const {
    return new CommandMODE(*this);
}
