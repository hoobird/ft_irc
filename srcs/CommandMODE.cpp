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
// 482 ERR_CHANOPRIVSNEEDED (done)
// 442 ERR_NOTONCHANNEL (Returned when a client tries to perform a channel-affecting command on a channel which the client isn’t a part of.)
// 401 ERR_NOSUCHNICK (Indicates that no client can be found for the supplied nickname. The text used in the last param of this message may vary.)
// 324 RPL_CHANNELMODEIS (sent to a client to inform them of the currently active modes of a channel)
// 329 RPL_CREATIONTIME (maybe cannot replicate cause of illegal function)
// 472 ERR_UNKNOWNMODE (Indicates that a mode character used by a client is not recognized by the server. The text used in the last param of this message may vary.)

// 467 ERR_KEYSET (Implemented for classic RFC compliant server, where if a key is set already, it needs to be removed first, dalnet simply overwrites with +k if pass was set alr)
// 346 RPL_INVITELIST (under client protocol)
// 347 RPL_ENDOFINVITELIST (likely paired with 346, check output, under client protocol)
// 325 RPL_UNIQOPIS (older implement, likely finds all ops in a channel and return nickname with relevant prefix attached. Check output to confirm)

// Channel Mode Parameters: <channel> {[+|-]|o|i|t|k|l} [<limit>] [<user>] [<ban mask>]

// need to do basic parsing (if + or -, then followed by whatever mode)

responseList CommandMODE::errorHandle(const ParsedMessage& message, const std::string clientFdStr, const std::string clientNick) {
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

bool needParameter(char commandType, char operation)
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
    mapModeFlags   parsedFlags;
	const std::string flags = message.parameters[1];
    char commandType;
    char operation = '\0'; // default behaviour

    // loop through commandType, if no command type to loop, there should be no valid modeParam.
	for (size_t i = 0; i < flags.size(); ++i) {
        char c = flags[i];
        if (c == '+' || c == '-')
            operation = c;
        else {
            std::pair<char, std::string> action;
            action.first = operation;
            commandType = c;
            if (commandType != '\0') {
                parsedFlags[commandType];
            }
            if (needParameter(commandType, operation) == true && i < message.parameters.size())
                action.second = message.parameters[i];
            action.second = "";
            parsedFlags[commandType].push_back(action);
        }
    }
    return parsedFlags;
}

responseList CommandMODE::execute(Client& client, const ParsedMessage& message) {
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
        resp["<mode>"] = targetChannel->displayModes(); // default seems to just display + if is query
        resp["<mode_params>"] = "";
        responses.push_back(resp);
        return responses; // rethink if I should return responses.
    }

    // Setting mode
    else {
        if (targetChannel->isOperator(client) == false) {
            // 482 ERR_CHANOPRIVSNEEDED
            singleResponse resp = createSingleResponse("482", clientFdStr);
            resp["<client>"] = clientNick;
            resp["<channel>"] = channelName;
            resp["<reason>"] = "You're not channel operator";
            responses.push_back(resp);
            return responses ;
        }
        mapModeFlags parsedFlags = parse(message);
		for (mapModeFlags::const_iterator mapIt = parsedFlags.begin(); mapIt != parsedFlags.end(); ++mapIt) {
            char modeChar = mapIt->first;
            size_t maxIter = (modeChar == 'o') ? std::min((size_t)3, mapIt->second.size()) : 1;
			for (size_t i = 0; i < maxIter; ++i) {
                char action = (mapIt->second)[i].first;
                const std::string param = (mapIt->second)[i].second;
	            switch (modeChar) {
                    case 'i':
                        if (action == '+') {
	                        targetChannel->setInviteMode(true);
                            std::cout << "+i" << std::endl;
	                    }
	                    else if (action == '-') {
	                        targetChannel->setInviteMode(false);
                            targetChannel->emptyInviteList();
                            std::cout << "-i" << std::endl;
	                    }
	                    break ;
	                case 't':
	                    if (action == '+') {
	                        targetChannel->setTopicRestrict(true);
                            std::cout << "+t" << std::endl;
	                    }
	                    else if (action == '-') {
	                        targetChannel->setTopicRestrict(false);
	                        targetChannel->setTopic("");
                            std::cout << "-t" << std::endl;
	                    }
	                    break ;
	                case 'k':
	                    if (action == '+') {
	                        targetChannel->setKey(param);
	                    }
	                    else if (action == '-') {
	                        if (param == targetChannel->getKey()) // does the param need to have the same key to trigger remove? NC seems yes, IRSSI seems no
	                            targetChannel->setKey("");
	                    }
	                    break ;
	                case 'o': { // this is done in a loop
                        Client* targetClient = dataStore.getClient(param);
                        if (!targetClient) { // check behaviour, this may get silently ignored.
                            // ERR_NOSUCHNICK 401
                            singleResponse resp = createSingleResponse("401", clientFdStr);
                            resp["<client>"] = clientNick;
                            resp["<nick>"] = param;
                            resp["<reason>"] = "No such nick/channel";
                            responses.push_back(resp);
                        }
	                    if (action == '+') {
                            targetChannel->addOperator(*targetClient);
	                    }
	                    else if (action == '-') {
                            targetChannel->removeOperator(*targetClient);
	                    }
	                    continue ;
                    }
	                case 'l':
	                    if (action == '+') {
                            std::istringstream iss(param);
                            int value;
                            if (iss >> value && value > 0) // only set if value is negative
	                            targetChannel->setLimit(value);
	                    }
	                    else if (action == '-') {
	                        targetChannel->setLimit(-1);
	                    }
	                    break ;
	                default:
                        // 472 ERR_UNKNOWNMODE
	                    singleResponse resp = createSingleResponse("472", clientFdStr);
	                    resp["<client>"] = clientNick;
	                    resp["<char>"] = mapIt->first;
	                    resp["<reason>"] = "is unknown mode char to me";
	                    responses.push_back(resp);
	                    break ;
	            }
			}
		}
	}
	return responses;
}

CommandBase* CommandMODE::clone() const {
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
