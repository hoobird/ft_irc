#include "AllCommands.hpp"

CommandTOPIC::CommandTOPIC(const DataStore& dataStore): CommandBase(), dataStore(dataStore)
{
}

CommandTOPIC::CommandTOPIC(const CommandTOPIC &other): CommandBase(), dataStore(other.dataStore)
{
}

CommandTOPIC::~CommandTOPIC()
{
}

// 461 ERR_NEEDMOREPARAMS	Not enough parameters given	If channel name is missing.
// 403 ERR_NOSUCHCHANNEL	No such channel	If channel does not exist.
// 442 ERR_NOTONCHANNEL	You're not on that channel	If client is not a member of the channel.
// 482 ERR_CHANOPRIVSNEEDED	You're not channel operator	If client tries to set topic but is not operator.
// 331 RPL_NOTOPIC	No topic is set	If channel has no topic (query only).
// 332 RPL_TOPIC	Topic reply	If channel has a topic (query only).
// TOPIC (custom reply)	Topic change notification	Notify all members when topic is set.
responseList CommandTOPIC::execute(Client& client, const ParsedMessage& message)
{
    responseList responses;
    const std::string clientFdsStr = client.getSocketFdString();
    const std::string clientNick = client.getClientPrefix();

    // firstCase : Param = channel  -> query topic
    // OR
    // secondCase : Param = channel + topic -> set topic
    // OR
    // thirdCase : Param = channel AND trailing = something  -> set topic
    // OR
    // fourthCase : Param = channel AND trailingExist = true but trailing = "" -> Unset topic
    bool firstCase = message.parameters.size() >= 1;
    bool secondCase = message.parameters.size() == 1 && !message.trailing.empty();
    bool thirdCase = message.parameters.size() >= 2;
    bool fourthCase = message.parameters.size() == 1 && message.trailingExists && message.trailing.empty();
    if (!firstCase) {
        singleResponse resp = createSingleResponse("461", clientFdsStr);
        resp["<client>"] = clientNick;
        resp["<command>"] = "TOPIC";
        resp["<reason>"] = "Not enough parameters";
        responses.push_back(resp);
        return responses;
    }
    if (!message.parameters[0].empty() && message.parameters[0][0] == '#') {
        Channel* findChannel = dataStore.getChannel(message.parameters[0]);
        if (!findChannel) {
            singleResponse resp = createSingleResponse("403", clientFdsStr);
            resp["<client>"] = clientNick;
            resp["<channel>"] = message.parameters[0];
            resp["<reason>"] = "No such channel";
            responses.push_back(resp);
            return responses;
        }
        if (findChannel->isMember(client) == false) {
            singleResponse resp = createSingleResponse("442", clientFdsStr);
            resp["<client>"] = clientNick;
            resp["<channel>"] = message.parameters[0];
            resp["<reason>"] = "You're not on that channel";
            responses.push_back(resp);
            return responses;
        }
        // set topic
        if (secondCase || thirdCase || fourthCase) {
            // need check here if commandMODE restricts topic setting to operator only.
            if (findChannel->getTopicRestrict() == true && findChannel->isOperator(client) == false) {
                singleResponse resp = createSingleResponse("482", clientFdsStr);
                resp["<client>"] = clientNick;
                resp["<channel>"] = message.parameters[0];
                resp["<reason>"] = "You're not channel operator";
                responses.push_back(resp);
                return responses;
            }
            std::string newTopic;
            if (message.parameters.size() > 1) {
                newTopic = message.parameters[1];
            }
            else if (!message.trailing.empty()) {
                newTopic = message.trailing;
            }
            findChannel->setTopic(newTopic);
            std::string memberFdsStr = intSetToCSVString(findChannel->getMembers());
            singleResponse resp = createSingleResponse("TOPIC", memberFdsStr);
            resp["<nick_sender>"] = clientNick;
            resp["<user_sender>"] = client.getUsername();
            resp["<host_sender>"] = client.getHostname();
            resp["<channel>"] = message.parameters[0];
            resp["<new_topic>"] = newTopic;
            responses.push_back(resp);
            // update topicUpdateTime and update topic author
            findChannel->setTopicAuthor(clientNick);
            findChannel->setTopicUpdateTime();
        }
        // just query (first case)
        else {
            if (findChannel->getTopic().empty()) {
                singleResponse resp = createSingleResponse("331", clientFdsStr);
                resp["<client>"] = clientNick;
                resp["<channel>"] = message.parameters[0];
                resp["<info>"] = "No topic is set";
                responses.push_back(resp);
                return responses;
            }
            else {
                singleResponse resp = createSingleResponse("332", clientFdsStr);
                resp["<client>"] = clientNick;
                resp["<channel>"] = message.parameters[0];
                resp["<topic>"] = findChannel->getTopic();
                responses.push_back(resp);

                resp = createSingleResponse("333", clientFdsStr);
                resp["<client>"] = clientNick;
                resp["<channel>"] = message.parameters[0];
                resp["<nick>"] = findChannel->getTopicAuthor();
                resp["<setat>"] = findChannel->getTopicUpdateTimeString();
                responses.push_back(resp);
                return responses;
            }
        }
    }
    return responses;
}


CommandBase* CommandTOPIC::clone() const
{
    return new CommandTOPIC(*this);
}
