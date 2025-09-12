#ifndef BOTBASE_HPP
#define BOTBASE_HPP

#include <string>
#include <vector>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <signal.h>


class BotBase {

    public:
        BotBase();
        virtual ~BotBase();
        struct MessageIN {
            std::string from;
            std::string message;
        };

        virtual std::string handle_server_message(BotBase::MessageIN messageIN) = 0;
        MessageIN validateMessage(const std::string& rawMessage);
        std::string prepareOutgoingMessage(const std::string& msg_receiver, const std::string& msg);

        void connect_to_server(const std::string& ip, const std::string& port, const std::string &pass);
        void disconnect_from_server();
        void run();

        // Getters
        int getBotFd() const;
        void shutdown();
        
        bool setupSignalHandler(void (*handler)(int));

        class BotException : public std::exception {
            private:
                std::string fullMessage;
            public:
                BotException(const std::string& msg) : fullMessage("[BotBase] " + msg) {}
                ~BotException() throw() {}
                virtual const char* what() const throw() {
                    return fullMessage.c_str();
                }
        };

    protected:
        int botsocket;
        std::string name;
        std::string hostname;
        bool running;
        fd_set readfds;
        
    private:
        std::string generateAuthMessage(const std::string &pass);
        BotBase(const BotBase &);
        BotBase& operator=(const BotBase&);
    };
    
#endif