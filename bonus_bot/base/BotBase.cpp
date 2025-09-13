#include "BotBase.hpp"
#include <csignal>
#include <iostream>

BotBase *BotBase::instance = NULL;

BotBase::BotBase(): botsocket(-1), running(false), messagebuffer("")
{
    instance = this;
    setupSignalHandler(BotBase::internalSignalHandler);
}

BotBase::~BotBase()
{
    disconnect_from_server();
}

std::vector<std::string> BotBase::parseMessages(const std::string& rawMessage) {
    std::vector<std::string> allMessages;
    std::string fullMessage = messagebuffer + rawMessage;
    messagebuffer.clear();
    size_t pos = fullMessage.find("\r\n");
    while (pos != std::string::npos) {
        std::string messageToProcess = fullMessage.substr(0, pos); // dont include \r\n
        fullMessage.erase(0, pos + 2); // remove processed message and \r\n from fullMessage
        if (messageToProcess.empty()) {
            pos = fullMessage.find("\r\n");
            continue; // skip empty messages
        }
        // Store or process the messageToProcess as needed
        allMessages.push_back(messageToProcess);
        pos = fullMessage.find("\r\n");
    }
    // Store any remaining partial message back to messagebuffer
    messagebuffer = fullMessage;
    // Return a vector of complete messages (this is just a placeholder, implement as needed)
    return allMessages;
}

BotBase::MessageIN BotBase::processMessage(const std::string &message){
    std::stringstream ss(message);
    std::string prefix, command, msg_receiver, msg;
    ss >> prefix >> command >> msg_receiver;
    if (ss.peek() == ' ')
        ss.get(); // consume the space
    std::getline(ss, msg);


    // check if command is PRIVMSG first
    if (command == "433") // Nickname already used, meaing bot already connected
    {
        running = false;
        throw BotException("Nickname already in use, bot might already be connected");
    }
    if (command != "PRIVMSG") {
        std::string err = "Server -> Bot: " + message;
        throw BotException(err.c_str());
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
    messageIN.source = nick_sender;
    messageIN.receiver = msg_receiver;
    messageIN.message = actual_msg;

    std::cout << "Debug: prefix='" << prefix << "', command='" << command << "', msg_receiver='" << msg_receiver << "', msg='" << actual_msg << "'" << std::endl;

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

std::string BotBase::processUserInput(const std::string &input)
{
    // Process input from Bot Admin on stdin
    // split by spaces first
    std::stringstream ss(input);
    std::vector<std::string> usercmds;
    std::string segment;
    while (std::getline(ss, segment, ' ')) {
        usercmds.push_back(segment);
    }
    std::string output;

    if (usercmds.size() < 1){
        return output;
    }
    if (usercmds[0] == "/help") {
        std::cout << "Available commands:\n"
                << "/help - Show this help message\n"
                << "/ping - check server is alive\n"
                << "/join <#channel> - Join a channel\n"
                << "/part <#channel> - Leave a channel\n"
                << "/quit - Disconnect and exit the bot"
                << std::endl;
    } else if (usercmds[0] == "/ping")  {
        output += "PING BotAdminCheck\r\n";
    } else if (usercmds[0] == "/join") {
        if (usercmds.size() < 2) {
            std::cout << "Usage: /join <#channel>" << std::endl;
        } else if (usercmds[1][0] != '#') {
            std::cout << "Channel name must start with '#'" << std::endl;
        } else {
            output += "JOIN " + usercmds[1] + "\r\n";
        }
    } else if (usercmds[0] == "/part") {
        if (usercmds.size() < 2) {
            std::cout << "Usage: /part <#channel>" << std::endl;
        } else {
            output += "PART " + usercmds[1] + "\r\n";
        }
    } else if (usercmds[0] == "/quit") {
        output += "QUIT :Bot shutting down\r\n";
        running = false;
    } else {
        std::cout << "Unknown command. Type /help for a list of commands." << std::endl;
    }
    return output;
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
        // send QUIT message before closing
        std::string quitMessage = "QUIT :Bot shutting down\r\n";
        send(botsocket, quitMessage.c_str(), quitMessage.length(), MSG_NOSIGNAL);
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
                try {
                    std::vector<std::string> parsedMessages = parseMessages(message);
                    for (std::vector<std::string>::iterator it = parsedMessages.begin(); it != parsedMessages.end(); ++it) {
                        BotBase::MessageIN messageIN = processMessage(*it);
                        std::pair<std::string,std::string> botresponse = handle_server_message(messageIN);
                        std::string finalresponse = prepareOutgoingMessage(botresponse.first, botresponse.second);
                        std::cout << "Bot -> Server: " << finalresponse << std::endl;
                        if (!finalresponse.empty()) {
                            ssize_t bytesSent = send(botsocket, finalresponse.c_str(), finalresponse.length(), 0);
                            if (bytesSent == -1) {
                                running = false;
                                throw BotException("Failed to send message to server");
                            }
                        }
                    }
                } catch (const BotException& e) {
                    std::cerr << e.what() << std::endl;
                    continue;
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
                std::string msgToSend = processUserInput(userInput);
                if (msgToSend.empty()) {
                    continue; // No message to send
                }
                ssize_t bytesSent = send(botsocket, msgToSend.c_str(), msgToSend.length(), MSG_NOSIGNAL);
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
