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

responseList CommandMODE::errorHandle(const ParsedMessage& message, const std::string clientFdStr, const std::string clientNick)
{
    responseList responses;

    // 461 ERR_NEEDMOREPARAMS
    if (message.parameters.size() < 1) {
        singleResponse resp = createSingleResponse("461", clientFdStr);
        resp["<client>"] = clientNick;
        resp["<command>"] = "MODE";
        resp["<reason>"] = "Not enough parameters";
        responses.push_back(resp);
        return responses;
    }

    // 501 ERR_UMODEUNKNOWNFLAG
    if (message.parameters[0][0] != '#') {
        singleResponse resp = createSingleResponse("501", clientFdStr);
        resp["<client>"] = clientNick;
        resp["<reason>"] = "This server does not support user modes";
        responses.push_back(resp);
        return responses;
    }
    return responses;
}

bool CommandMODE::needParameter(char commandType, char operation)
{
    switch (commandType) {
        case 'k':
            return true;
        case 'o':
            return true;
        case 'l':
            return (operation == '+');
        case 'i':
            return false;
        case 't':
            return false;
        default:
            return false;
    }
}

CommandMODE::mapModeFlags CommandMODE::parse(const ParsedMessage& message)
{
    // typedef std::map<char, std::vector<std::pair<char, std::string> > >
    // map = parsedFlags; map[key] = char flags[i]; map[value] = actionList<action>; action.first(char) = "+"/"-"/'0'; action.second(string) = modeParams (i.e "password", limit value, UserNick)
    mapModeFlags   parsedFlags; //std::map
    std::vector<std::string>::const_iterator paramIt = message.parameters.begin() + 1;
	const std::string flags = *paramIt;
    ++paramIt;
    char commandType;
    char operation = '\0'; // default behaviour
    for (std::string::const_iterator flagIt = flags.begin(); flagIt != flags.end(); ++flagIt) {
        char c = *flagIt;
        if (c == '+' || c == '-') {
            operation = c; // +/-
        }
        else {
            commandType = c; // tilko
            std::pair<char, std::string> action;
            action.first = operation;
            if (commandType != '\0') {
                parsedFlags[commandType];
            }
            if (needParameter(commandType, operation) == true && paramIt != message.parameters.end()) {
                action.second = *paramIt;
                ++paramIt;
            }
            else {
                action.second = "";
            }
            if (commandType != '\0') {
                parsedFlags[commandType].push_back(action);
            }
        }
    }
    return parsedFlags;
}

bool CommandMODE::isValidModeChar(char modeChar)
{
    return (modeChar == 't' || modeChar == 'i'
        || modeChar == 'l' || modeChar == 'o' || modeChar == 'k');
}

void CommandMODE::parseFlagCollector(std::string &flagCollector)
{
    std::string::iterator flagIt;
    char currState = '0';

    for (flagIt = flagCollector.begin(); flagIt != flagCollector.end(); ++flagIt) {
        if (*flagIt == '+' || *flagIt == '-') {
            if (currState != *flagIt) {
                currState = *flagIt;
                continue ;
            }
            flagCollector.erase(flagIt);
        }
    }
}
// 461 ERR_NEEDMOREPARAMS (done)
// 501 ERR_UMODEUNKNOWNFLAG (done)
// 403 ERR_NOSUCHCHANNEL (done)
// 482 ERR_CHANOPRIVSNEEDED (done)
// 442 ERR_NOTONCHANNEL (Returned when a client tries to perform a channel-affecting command on a channel which the client isn’t a part of.)
// 401 ERR_NOSUCHNICK (Indicates that no client can be found for the supplied nickname. The text used in the last param of this message may vary.)
// 324 RPL_CHANNELMODEIS (sent to a client to inform them of the currently active modes of a channel)
// 329 RPL_CREATIONTIME (maybe cannot replicate cause of illegal function)
// 329 RPL_CREATIONTIME (maybe cannot replicate cause of illegal function)
// 472 ERR_UNKNOWNMODE (Indicates that a mode character used by a client is not recognized by the server. The text used in the last param of this message may vary.)

// 467 ERR_KEYSET (Implemented for classic RFC compliant server, where if a key is set already, it needs to be removed first, dalnet simply overwrites with +k if pass was set alr)
// 346 RPL_INVITELIST (under client protocol)
// 347 RPL_ENDOFINVITELIST (likely paired with 346, check output, under client protocol)
// 325 RPL_UNIQOPIS (older implement, likely finds all ops in a channel and return nickname with relevant prefix attached. Check output to confirm)

// Channel Mode Parameters: <channel> {[+|-]|o|i|t|k|l} [<limit>] [<user>] [<ban mask>]

// need to do basic parsing (if + or -, then followed by whatever mode)
responseList CommandMODE::execute(Client& client, const ParsedMessage& message)
{
    responseList responses;
    const std::string clientNick = client.getClientPrefix();
    const std::string clientFdStr = client.getSocketFdString();

    responses = errorHandle(message, clientFdStr, clientNick);
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

    // Is query (client doesn't have to be in channel or operator)
    if (message.parameters.size() == 1) {
        singleResponse resp = createSingleResponse("324", clientFdStr);
        resp["<client>"] = clientNick;
        resp["<channel>"] = channelName;
        resp["<mode>"] = targetChannel->displayModes();
        resp["<mode_params>"] = "";
        responses.push_back(resp);
        return responses;
    }
    // Setting mode
    // param > 1 (at least 2)
    else {
        if (targetChannel->isMember(client) == false) {
            singleResponse resp = createSingleResponse("442", clientFdStr);
            resp["<client>"] = clientNick;
            resp["<channel>"] = channelName;
            resp["<reason>"] = "You're not on that channel";
            responses.push_back(resp);
            return responses;
        }
        if (targetChannel->isOperator(client) == false) {
            singleResponse resp = createSingleResponse("482", clientFdStr);
            resp["<client>"] = clientNick;
            resp["<channel>"] = channelName;
            resp["<reason>"] = "You're not channel operator";
            responses.push_back(resp);
            return responses ;
        }
        mapModeFlags parsedFlags = parse(message);
        std::string flagCollector;
        std::vector<std::string> paramCollector;
		for (mapModeFlags::const_iterator mapIt = parsedFlags.begin(); mapIt != parsedFlags.end(); ++mapIt) {
            char modeChar = mapIt->first;
            std::vector<std::pair<char, std::string> > action = mapIt->second;
            if (action[0].second.empty() && isValidModeChar(modeChar)) {
                if (modeChar == 'i') {
                    if (action[0].first == '+') {
                        targetChannel->setInviteMode(true);
                        flagCollector += "+i";
                    }
                    else if (action[0].first == '-') {
                        targetChannel->setInviteMode(false);
                        targetChannel->emptyInviteList();
                        flagCollector += "-i";
                    }
                }
                else if (modeChar == 't') {
                    if (action[0].first == '+') {
                        targetChannel->setTopicRestrict(true);
                        flagCollector += "+t";
                    }
                    else if (action[0].first == '-') {
                        targetChannel->setTopicRestrict(false);
                        targetChannel->setTopic("");
                        flagCollector += "-t";
                    }
                }
                else {
                    std::ostringstream oss;
                    oss << "MODE " << action[0].first << modeChar;
                    singleResponse resp = createSingleResponse("461", clientFdStr);
                    resp["<client>"] = clientNick;
                    resp["<command>"] = oss.str();
                    resp["<reason>"] = "Not enough parameters";
                    responses.push_back(resp);
                    return responses;
                }

            }
            else {
                std::string singleCallParam = action[0].second;
                if (modeChar == 'k') {
                    if (singleCallParam.size() > 23) {
                        singleCallParam.resize(23);
                    }
                    if (action[0].first == '+') {
                        targetChannel->setKey(singleCallParam);
                        flagCollector += "+k";
                        paramCollector.push_back(singleCallParam);
                    }
                    else if (action[0].first == '-') {
                        if (singleCallParam == targetChannel->getKey()) {
                            targetChannel->setKey("");
                            flagCollector += "-k";
                            paramCollector.push_back(singleCallParam);

                        }
                    }
                }
                else if (modeChar == 'l') {
                    if (action[0].first == '+') {
                        std::istringstream iss(singleCallParam);
                        int value;
                        if (iss >> value && value > 0) {
                            targetChannel->setLimit(value);
                            flagCollector += "+l";
                            paramCollector.push_back(singleCallParam);
                        }
                    }
                    else if (action[0].first == '-') {
                        targetChannel->setLimit(-1);
                        flagCollector += "-l";
                        paramCollector.push_back(singleCallParam);
                    }
                }
                else if (modeChar == 'o') {
                    std::vector<std::pair<char, std::string> >::const_iterator it;
                    for (it = action.begin(); it != action.end(); ++it) {
                        std::string multiCallParam = it->second;
                        Client* targetClient = dataStore.getClient(multiCallParam); // may not be correct
                        if (!targetClient) {
                            // ERR_NOSUCHNICK 401
                            singleResponse resp = createSingleResponse("401", clientFdStr);
                            resp["<client>"] = clientNick;
                            resp["<nick>"] = multiCallParam;
                            resp["<reason>"] = "No such nick/channel";
                            responses.push_back(resp);
                            continue;
                        }
                        if (it->first == '+') {
                            targetChannel->addOperator(*targetClient);
                            flagCollector += "+o";
                            paramCollector.push_back(multiCallParam);
                        }
                        else if (it->first == '-') {
                            targetChannel->removeOperator(*targetClient);
                            flagCollector += "-o";
                            paramCollector.push_back(multiCallParam);
                        }
                    }
                }
                else {
                    std::ostringstream oss;
                    oss << "Unknown mode character " << modeChar;
                    singleResponse resp = createSingleResponse("472", clientFdStr);
                    resp["<client>"] = clientNick;
                    resp["<char>"] = mapIt->first;
                    resp["<reason>"] = oss.str(); // following dalnet behaviour
                    responses.push_back(resp);
                }
            }
        }
        if (!flagCollector.empty())
        {
            std::stringstream ss;
            for (std::vector<std::string>::iterator strIt = paramCollector.begin(); strIt != paramCollector.end(); ++strIt) {
                    ss << *strIt << " ";
            }
            // return MODE acknowledgement response
            // >> :anteo!~anteo@5626-2a9c-92a4-503c-675e.149.203.ip MODE #helluu +i+k pass
            parseFlagCollector(flagCollector);
            singleResponse resp = createSingleResponse("MODE", clientFdStr);
            resp["<nick_sender>"] = clientNick;
            resp["<user_sender>"] = client.getUsername();
            resp["<host_sender>"] = client.getHostname();
            resp["<channel>"] = message.parameters[0];
            resp["<flag>"] = flagCollector;
            resp["<param>"] = ss.str();
            responses.push_back(resp);
        }
    }
    return responses;
}

CommandBase* CommandMODE::clone() const
{
    return new CommandMODE(*this);
}


// (1) // +i/-i: doesn't take extra params
// (1) // +t/-t: doesn't take extra params
// (2) // +l/-l: only +l takes next parameter that needs to be a numerical number, if yes, apply flag to channel; if no, it will be consumed. (theory: don't check l flags again after first instance)
// (3) // +o/-o: takes next parameter, check if it's channel member name: if yes, apply flag to member; otherwise the param is consumed. (theory: check all subsequent flags and implement them all if valid)
// (3) // +k/-k: takes next parameter regardless of character type as password, even if they are mode flags (theory: don't check k flags again after first instance)


// flag params are the next available message.param[*]: -l -l
// -l +l +l =  -l (takes first instance of mode flag, delim by space) second priority?
// ---------+l = +l (takes the closest symbol to the mode flag) top priority?


// save last symbol encountered
// last symbol encountered + valid letters = flag
//
