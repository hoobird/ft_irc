#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include "CommandFactory.hpp"
#include "CommandBase.hpp"
#include "Client.hpp"
#include <string>
#include <vector>
#include <set>

class CommandHandler {
    public:
        CommandHandler(CommandFactory& cmdFactory);
        ~CommandHandler();

        responseList handleCommand(Client& client, const ParsedMessage& parsed);
        void sendNumericResponse(Client& client, int numeric, const std::vector<std::string>& params);

    private:
        CommandFactory &cmdFactory;

        std::set<std::string> allowedPreAuthCommands;

        // make ParseMessage for ERR_NOTREGISTERED (451) 
        responseList createNotRegisteredMessage();
        responseList createUnknownCommandMessage(const Client &client, const std::string &command);

        // OCF
        CommandHandler();
        CommandHandler(const CommandHandler& other);
        CommandHandler& operator=(const CommandHandler& other);
};

#endif
