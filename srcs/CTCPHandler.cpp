#include "bonus/CTCPHandler.hpp"

CTCPHandler::CTCPHandler()
{
}

CTCPHandler::~CTCPHandler()
{
}

bool CTCPHandler::isCTCPCommand(const ParsedMessage &message)
{
    // CTCP messages are sent as PRIVMSG or NOTICE with the message text enclosed in ASCII 0x01 characters
    if (message.trailing.size() < 2) { // at least two 0x01 characters
        return false;
    }
    if (message.trailing[0] == 0x01 && message.trailing[message.trailing.size() - 1] == 0x01) {
        return true;
    }
    return false;
}

responseList CTCPHandler::handleCTCPCommand(Client &sender, Client &receiver, const ParsedMessage &message)
{
    responseList responses;

    std::string receiverFd = receiver.getSocketFdString();

    singleResponse resp;
    resp["<numeric>"] = "PRIVMSG";
    resp["<clientsToSend>"] = receiverFd;
    resp["<nick_sender>"] = sender.getNickname();
    resp["<user_sender>"] = sender.getUsername();
    resp["<host_sender>"] = sender.getHostname();
    resp["<msg_receiver>"] = receiver.getNickname();
    resp["<msg>"] = message.trailing;
    responses.push_back(resp);

    return responses;
}
