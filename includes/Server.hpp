#ifndef SERVER_HPP
#define SERVER_HPP

#include "AuthService.hpp"
#include "DataStore.hpp"
#include "NetworkManager.hpp"
#include "NetworkErrors.hpp"
#include "MessageParser.hpp"
#include "MessageBuilder.hpp"
#include "CommandHandler.hpp"
#include "CommandFactory.hpp"
#include "AllCommands.hpp"
#include <string>
#include <iostream>


class Server: public AuthService
{
    public:
        Server(int port, const std::string& password);
        ~Server();

        void start();
        void shutdown();

        void runEventLoop();

        // Getters
        int getPort() const;
        std::string getPortString() const;

        // AuthService interface (only way to access password)
        virtual bool validatePassword(const std::string& input) const;

    private:
        void sleepCountdown(); // for testing only, remove later
        void setupCommandFactory(); // register commands to factory
        void handleNewConnection();
        void handleClientEvent(int fd);
        void processClientMessages(int clientFd, std::string& bufferString);

        std::string serverName;
        bool running;
        int port;
        std::string password;
        DataStore dataStore;
        NetworkManager networkMan;
        MessageParser msgParser;
        CommandFactory cmdFactory;
        CommandHandler cmdHandler;
        MessageBuilder msgBuilder;

        // Useless OCF
        Server();
        Server(const Server& other);
        Server& operator=(const Server& other);
};

#endif