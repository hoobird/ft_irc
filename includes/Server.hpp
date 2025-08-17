#ifndef SERVER_HPP
#define SERVER_HPP

#include "DataStore.hpp"
#include "NetworkManager.hpp"
// #include "CommandHandler.hpp"
// #include "MessageParser.hpp"
#include "NetworkErrors.hpp"
#include <string>
#include <iostream>


class Server {
    public:
        Server(int port, const std::string& password);
        ~Server();

        void start();
        void shutdown();

        void runEventLoop();



    private:
        DataStore dataStore;
        NetworkManager networkMan;
        // CommandHandler cmdHandler;
        // MessageParser cmdParser;

        bool running;
        int socketFd;


        // Useless OCF
        Server();
        Server(const Server& other);
        Server& operator=(const Server& other);
};

#endif