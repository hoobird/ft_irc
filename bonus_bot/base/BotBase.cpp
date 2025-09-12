#include "BotBase.hpp"
#include <csignal>
#include <iostream>

BotBase *BotBase::instance = NULL; // initialize static member

BotBase::BotBase(): botsocket(-1), running(false)
{
    instance = this;
    setupSignalHandler(BotBase::internalSignalHandler);
}

BotBase::~BotBase()
{
    disconnect_from_server();
}
#include <cstdio>
BotBase::MessageIN BotBase::validateMessage(const std::string &rawMessage)
{
    // ":<nick_sender>!<user_sender>@<host_sender> PRIVMSG <msg_receiver> :<msg>";
    if (rawMessage.empty() || rawMessage.size() < 2) {
        throw BotException("Empty message received");
    }
    // Debug: print raw message and hex values
    std::cout << "Raw message: '" << rawMessage << "'" << std::endl;
    std::cout << "Hex: ";
    for (size_t i = 0; i < rawMessage.size(); ++i) {
        printf("%02X ", static_cast<unsigned char>(rawMessage[i]));
    }
    printf("\n");
    // we only need nick_sender and msg
    // only repond to PRIVMSG
    // remove trailing \r\n if present in rawMessage
    std::string trimmedMessage = rawMessage;
    if (rawMessage[rawMessage.length() - 1] == '\n' && rawMessage[rawMessage.length() - 2] == '\r') {
        trimmedMessage = rawMessage.substr(0, rawMessage.length() - 2);
    } else {
        throw BotException("Invalid message format: missing CRLF");
    }
    std::stringstream ss(trimmedMessage);
    std::string prefix, command, msg_receiver, msg;
    ss >> prefix >> command >> msg_receiver;
    if (ss.peek() == ' ')
        ss.get(); // consume the space
    std::getline(ss, msg);

    std::cout << "Debug: prefix='" << prefix << "', command='" << command << "', msg_receiver='" << msg_receiver << "', msg='" << msg << "'" << std::endl;

    // check if command is PRIVMSG first
    if (command == "433") // Nickname already used, meaing bot already connected
    {
        running = false;
        throw BotException("Nickname already in use, bot might already be connected");
    }
    if (command != "PRIVMSG") {
        throw BotException("Only handling PRIVMSG");
    }
    if (prefix.empty() || prefix[0] != ':' || prefix.find('!') == std::string::npos) {
        throw BotException("Invalid message prefix format");
    }
    std::string nick_sender = prefix.substr(1, prefix.find('!') - 1);
    if (msg.empty() || msg[0] != ':' || msg.length() < 2) {
        throw BotException("Invalid message format");
    }
    std::string actual_msg = msg.substr(1); // Remove leading ':'
    MessageIN messageIN;
    messageIN.from = nick_sender;
    messageIN.message = actual_msg;
    return messageIN;
}

std::string BotBase::prepareOutgoingMessage(const std::string &msg_receiver, const std::string &msg)
{
    // first change all \n in msg to \r\n
    //
    std::string prefix = ":" + name + "!" + name + "@" + hostname + " NOTICE " + msg_receiver + " :";

    std::stringstream msgstream(msg);
    // Split by newline and prepare each line
    std::string line;
    std::stringstream finalmessage;
    while (std::getline(msgstream, line)) {
        if (!line.empty()) {
            finalmessage << prefix << line << "\r\n";
        }
    }
    return finalmessage.str();
}

void BotBase::connect_to_server(std::vector<std::string> args)
{
    const std::string ip = args[0];
    const std::string port = args[1];
    const std::string pass = args[2];
    // setup socket
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0; // client: no AI_PASSIVE

    struct addrinfo *servinfo, *p;
    // Use getaddrinfo just in case of hostname and supports both v4 and v6
    int status = getaddrinfo(ip.c_str(), port.c_str(), &hints, &servinfo);
    if (status != 0) {
        // gai_strerror: Convert error return from getaddrinfo() to a string.
        throw BotException("getaddrinfo error: " + std::string(gai_strerror(status)));
    }
    botsocket = -1;
    for (p = servinfo; p != NULL; p = p->ai_next) {
        int fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (fd < 0)
            continue;
        if (connect(fd, p->ai_addr, p->ai_addrlen) == 0) {
            botsocket = fd;
            break;
        }
        close(fd);
    }
    freeaddrinfo(servinfo);
    if (botsocket == -1) {
        throw BotException("Failed to connect to server");
    }

    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(local_addr);
    if (getsockname(botsocket, (struct sockaddr*)&local_addr, &addr_len) != 0) {
        close(botsocket);
        botsocket = -1;
        throw BotException("getsockname failed");
    }
    hostname = inet_ntoa(local_addr.sin_addr);
    // Send authentication message
    std::string authMessage = generateAuthMessage(pass);
    ssize_t bytesSent = send(botsocket, authMessage.c_str(), authMessage.length(), 0);
    if (bytesSent == -1) {
        close(botsocket);
        botsocket = -1;
        throw BotException("Failed to send authentication message");
    }
    running = true;

}

void BotBase::disconnect_from_server()
{
    if (botsocket != -1) {
        close(botsocket);
    }
    botsocket = -1;
}

void BotBase::run()
{
    while (running)
    {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds); // Monitor standard input as well
        FD_SET(botsocket, &readfds);
        int max_fd = (botsocket > STDIN_FILENO) ? botsocket : STDIN_FILENO;
        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (running == false) {
            break; // Exit loop if shutdown was called
        }
        if (activity < 0) {
            throw BotException("select error");
        }
        if (FD_ISSET(botsocket, &readfds)) {
            char buffer[512];
            memset(buffer, 0, sizeof(buffer));
            ssize_t bytesRead = recv(botsocket, buffer, sizeof(buffer), 0);
            if (bytesRead > 0) {
                std::string message = std::string(buffer, bytesRead > 0 ? bytesRead : 0);
                BotBase::MessageIN messageIN;
                try {
                    messageIN = validateMessage(message);
                } catch (const BotException& e) {
                    std::cerr << e.what() << std::endl;
                    continue;
                }
                std::string botresponse = handle_server_message(messageIN);
                botresponse = prepareOutgoingMessage(messageIN.from, botresponse);
                if (!botresponse.empty()) {
                    ssize_t bytesSent = send(botsocket, botresponse.c_str(), botresponse.length(), 0);
                    if (bytesSent == -1) {
                        throw BotException("Failed to send message to server");
                    }
                }
            } else if (bytesRead == 0) {
                // Connection closed by server
                disconnect_from_server();
                break;
            } else {
                throw BotException("recv error");
            }
        }
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            std::string userInput;
            if (std::getline(std::cin, userInput)) {
                std::cout << "Sending user input: " << userInput << std::endl; // Debug output
                userInput += "\r\n"; // IRC messages end with CRLF
                ssize_t bytesSent = send(botsocket, userInput.c_str(), userInput.length(), 0);
                if (bytesSent == -1) {
                    throw BotException("Failed to send message to server");
                }
            } else {
                // EOF or error on stdin
                disconnect_from_server();
                break;
            }
        }
    }

}

int BotBase::getBotFd() const
{
    return botsocket;
}

void BotBase::shutdown()
{
    running = false;
    write(STDOUT_FILENO, "\r\n", 2);
    std::cout << "Shutdown signal received, disconnecting..." << std::endl;
}

bool BotBase::setupSignalHandler(void (*handler)(int)) {
    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) != 0 ||
        sigaction(SIGQUIT, &sa, NULL) != 0) {
        std::cerr << "Error: Unable to set-up signal handlers." << std::endl;
        return false;
    }
    return true;
}

void BotBase::internalSignalHandler(int signum) {
    if (signum == SIGINT || signum == SIGQUIT) {
        if (BotBase::instance) {
            BotBase::instance->shutdown();
        }
    }
}

std::string BotBase::generateAuthMessage(const std::string &pass)
{
    std::string fullstring;
    fullstring = "PASS " + pass + "\r\n";
    fullstring += "NICK " + name + "\r\n";
    fullstring += "USER " + name + " 0 * :" + name + "\r\n";
    return fullstring;
}

std::vector<std::string> parseArgs(int argc, char **argv)
{
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(std::string(argv[i]));
    }
    return args;
}
