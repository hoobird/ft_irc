#ifndef SERVER_HPP
#define SERVER_HPP

#include "AuthService.hpp"
#include "DataStore.hpp"
#include "NetworkManager.hpp"
// #include "CommandHandler.hpp"
// #include "MessageParser.hpp"
#include "NetworkErrors.hpp"
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
        void sleepCountdown();
        void handleNewConnection();
        void handleClientEvent(int fd);
        void processClientMessages(int fd, std::string& bufferString);

        bool running;
        int port;
        std::string password;
        DataStore dataStore;
        NetworkManager networkMan;
        // CommandHandler cmdHandler;
        // MessageParser cmdParser;


        // Useless OCF
        Server();
        Server(const Server& other);
        Server& operator=(const Server& other);
};

#endif