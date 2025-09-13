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
        std::vector<std::string> parseMessages(const std::string& rawMessage);
        MessageIN processMessage(const std::string &message);
        std::string prepareOutgoingMessage(const std::string& msg_receiver, const std::string& msg);

        std::string processUserInput(const std::string &input);

        void connect_to_server(std::vector<std::string> args);
        void disconnect_from_server();
        void run();

        // Getters
        int getBotFd() const;
        void shutdown();


        static void internalSignalHandler(int signum);

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
        bool running;
        std::string name;
        std::string hostname;
        std::string messagebuffer;
        fd_set readfds;
        static BotBase *instance;

    private:
        bool setupSignalHandler(void (*handler)(int));
        std::string generateAuthMessage(const std::string &pass);
        BotBase(const BotBase &);
        BotBase& operator=(const BotBase&);
    };

    std::vector<std::string> parseArgs(int argc, char** argv);

#endif
