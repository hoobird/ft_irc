#include "AllCommands.hpp"

CommandJOIN::CommandJOIN(DataStore& dataStore)
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
    const std::string clientFdStr = client.getSocketFdString();
    const std::string clientNick = client.getClientPrefix();

    if (message.parameters.size() < 1) {
        // ERR_NEEDMOREPARAMS (461)
        singleResponse resp = createSingleResponse("461", clientFdStr);
        resp["<client>"] = clientNick;
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

    // store channelNames and channelKeys into std::map
    // note: inherent property of std::map to ensure unique keys in key-value pair
    std::map<std::string, std::string> mapChannelKey;
    for (std::vector<std::string>::size_type i = 0; i < channelNames.size(); ++i) {
        if (channelNames.size() <= channelKeys.size())
            mapChannelKey.insert(std::make_pair(channelNames[i], channelKeys[i]));
        else // if channelNames.size() > channelKeys.size()
            mapChannelKey.insert(std::make_pair(channelNames[i], std::string()));
    }

    for (std::vector<std::string>::size_type i = 0; i < channelNames.size(); ++i)  {
        Channel* findChannel = dataStore.getChannel(channelNames[i]);
        singleResponse resp;
        // fix: do not create local Channel object that gets destroyed
        if (!findChannel) {
            std::cout << "Channel does not exist...\n";
            Channel* channel = new Channel(channelNames[i]); // create on heap
            dataStore.addChannel(channel); // add pointer to store
            channel->addMember(client); // use pointer
            channel->addOperator(client); // use pointer

            // // std::cout << "-- I need to return a success response here --\n";
            std::string memberFds = intSetToCSVString(dataStore.getChannel(channelNames[i])->getMembers());
            resp = createSingleResponse("JOIN", memberFds);
            resp["<nick_sender>"] = clientNick;
            resp["<user_sender>"] = client.getUsername();
            resp["<host_sender>"] = client.getHostname();
            // resp["<channel>"] = findChannel->getName(); // in the case that channel exist
            resp["<channel>"] = message.parameters[0];
            responses.push_back(resp);

            // RPL_NAMREPLY (353)
            // >> :halcyon.il.us.dal.net 353 elfoo = #lobby :elfoo LDa224 Guest39645
            std::string memberNick;
            std::stringstream ss;

            // loop through member list to end(), if the member is also operator, append @
            std::set<int> membersList = dataStore.getChannel(channelNames[i])->getMembers();
            for (std::set<int>::const_iterator it = membersList.begin(); it != membersList.end(); ++it) {
                std::set<int> operatorList = dataStore.getChannel(channelNames[i])->getOperators();
                memberNick = dataStore.getClient(*it)->getNickname();
                if (operatorList.find(*it) != operatorList.end()) {
                    memberNick = "@" + memberNick;
                }
                if (it != membersList.begin()) {
                    ss << " ";
                }
                ss << memberNick;
            }
            resp = createSingleResponse("353", clientFdStr);
            resp["<client>"] = clientNick;
            resp["<channel>"] = message.parameters[0];
            resp["<nicks>"] = ss.str();
            responses.push_back(resp);

            // RPL_ENDOFNAMES (366)
            // >> :halcyon.il.us.dal.net 366 elfoo #lobby :End of /NAMES list.
            resp = createSingleResponse("366", clientFdStr);
            resp["<client>"] = clientNick;
            resp["<channel>"] = message.parameters[0];
            resp["<info>"] = "End of /NAMES list.";
            responses.push_back(resp);
        }
        else
        {
            std::cout << "Channel exists, proceeding checks...\n";
            int channelCount = dataStore.countChannelsForClient(client);
            std::string clientMaxChannels = getLimitString(LIMITS_MAXCHANNELS);
            std::istringstream iss(clientMaxChannels);
            int maxChannels;
            iss >> maxChannels;
            if (channelCount >= maxChannels) {
                resp = createSingleResponse("405", clientFdStr);
                resp["<client>"] = clientNick;
                resp["<channel>"] = findChannel->getName();
                resp["<reason>"] = "You have joined too many channels";
                responses.push_back(resp);
                continue ;
            }
            // if channel does not need password, but password is provided, return success; password is ignored
            if (findChannel->getKey() != "" && mapChannelKey[channelNames[i]] != findChannel->getKey()) {
                // if channel is password-protected, and password is wrong, return false
                resp = createSingleResponse("475", clientFdStr);
                resp["<client>"] = clientNick;
                resp["<channel>"] = findChannel->getName();
                resp["<reason>"] = "Cannot join channel (+k)";
                responses.push_back(resp);
                continue ;
            }
            if (findChannel->getInviteMode() == true && findChannel->isMemberInvited(client) == false) {
                resp = createSingleResponse("473", clientFdStr);
                resp["<client>"] = clientNick;
                resp["<channel>"] = findChannel->getName();
                resp["<reason>"] = "Cannot join channel (+i)";
                responses.push_back(resp);
                continue ;
                // invite overrides bans (if a user is banned from an invite-only channel, if they are given invite, they can join)
            }
            if (findChannel->getLimit() != -1 && static_cast<int>(findChannel->getMembers().size()) >= findChannel->getLimit()) {
                resp = createSingleResponse("471", clientFdStr);
                resp["<client>"] = clientNick;
                resp["<channel>"] = findChannel->getName();
                resp["<reason>"] = "Cannot join channel (+l)";
                responses.push_back(resp);
                continue ;
                // TODO: set arbitrary max users value; to manage in CommandMODE (both user and server)
            }
            std::cout << "Channel exists, continuing...\n";

            findChannel->addMember(client);
            // client joining existing channel, inform existing channel members of new member.
            // if client’s JOIN command to the server is successful, server returns a JOIN message
            // >> :elfoo!~elfoo@5626-2a9c-92a4-503c-675e.149.203.ip JOIN :#lobby
            std::string memberFds = intSetToCSVString(findChannel->getMembers());
            resp = createSingleResponse("JOIN", memberFds);
            resp["<nick_sender>"] = clientNick;
            resp["<user_sender>"] = client.getUsername();
            resp["<host_sender>"] = client.getHostname();
            resp["<channel>"] = findChannel->getName();
            responses.push_back(resp);

            //RPL_TOPIC (332)
            // >> :halcyon.il.us.dal.net 332 elfoo #lobby :miss you :P
            // TODO: should we always print it, or should we only print it if topic exist?
            resp = createSingleResponse("332", clientFdStr);
            resp["<client>"] = clientNick;
            resp["<channel>"] = findChannel->getName();
            resp["<topic>"] = findChannel->getTopic();
            responses.push_back(resp);

            // RPL_NAMREPLY (353)
            // >> :halcyon.il.us.dal.net 353 elfoo = #lobby :elfoo LDa224 Guest39645
            std::string memberNick;
            std::stringstream ss;

            // loop through member list to end(), if the member is also operator, append @
            std::set<int> membersList = findChannel->getMembers();
            for (std::set<int>::const_iterator it = membersList.begin(); it != membersList.end(); ++it) {
                std::set<int> operatorList = findChannel->getOperators();
                memberNick = dataStore.getClient(*it)->getNickname();
                if (operatorList.find(*it) != operatorList.end()) {
                    memberNick = "@" + memberNick;
                }
                if (it != membersList.begin()) {
                    ss << " ";
                }
                ss << memberNick;
            }

            resp = createSingleResponse("353", clientFdStr); // check to see if it sends to just the singular client or the entire channel members.
            resp["<client>"] = clientNick;
            resp["<channel>"] = findChannel->getName();
            resp["<nicks>"] = ss.str();
            responses.push_back(resp);

            // RPL_ENDOFNAMES (366)
            // >> :halcyon.il.us.dal.net 366 elfoo #lobby :End of /NAMES list.
            resp = createSingleResponse("366", clientFdStr);
            resp["<client>"] = clientNick;
            resp["<channel>"] = findChannel->getName();
            resp["<info>"] = "End of /NAMES list.";
            responses.push_back(resp);
        }
    }
    return responses;
}

CommandBase* CommandJOIN::clone() const {
    return new CommandJOIN(*this);
}
