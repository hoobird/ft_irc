#include "CommandHandler.hpp"

CommandHandler::CommandHandler(CommandFactory& cmdFactory): cmdFactory(cmdFactory)
{
    allowedPreAuthCommands.insert("NICK");
    allowedPreAuthCommands.insert("USER");
    allowedPreAuthCommands.insert("PASS");
}

CommandHandler::~CommandHandler()
{
}

responseList CommandHandler::handleCommand(Client &client, const ParsedMessage &parsed)
{
    // Unregistered clients can only use NICK, USER, and PASS commands
    if (!client.isRegistered() && allowedPreAuthCommands.find(parsed.command) == allowedPreAuthCommands.end()) {
        return createNotRegisteredMessage();
    }
    CommandBase* command = cmdFactory.cloneCommand(parsed.command);
    if (!command) {
        // Command not found ERR_UNKNOWNCOMMAND
        return createUnknownCommandMessage(client, parsed.command);
    }
    responseList responses = command->execute(client, parsed);
    delete command; // free the cloned command
    return responses;
}

responseList CommandHandler::createNotRegisteredMessage()
{
    responseList responses;
    singleResponse response;
    response["<numeric>"] = "451"; // ERR_NOTREGISTERED
    response["<client>"] = "*";
    response["<reason>"] = "You must finish connecting with another nickname first.";
    responses.push_back(response);
    return responses;
}

responseList CommandHandler::createUnknownCommandMessage(const Client &client, const std::string &command)
{
    responseList responses;
    singleResponse response;
    response["<numeric>"] = "421"; // ERR_UNKNOWNCOMMAND
    response["<client>"] = client.getNickname().empty() ? "*" : client.getNickname(); // if no nick, use "*"
    response["<command>"] = command;
    response["<reason>"] = "Unknown command";
    responses.push_back(response);
    return responses;
}