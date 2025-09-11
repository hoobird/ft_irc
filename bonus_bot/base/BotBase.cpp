#include "BotBase.hpp"

BotBase::BotBase(): botsocket(-1), connected(false)
{}

BotBase::~BotBase()
{
    if (connected) {
        disconnect_from_server();
    }
}

BotBase::MessageIN BotBase::validateMessage(const std::string &rawMessage)
{
    // ":<nick_sender>!<user_sender>@<host_sender> PRIVMSG <msg_receiver> :<msg>";
    if (rawMessage.empty()) {
        throw BotException("Empty message received");
    }
    // we only need nick_sender and msg
    // only repond to PRIVMSG
    std::stringstream ss(rawMessage);
    std::string prefix, command, msg_receiver, msg;
    ss >> prefix >> command >> msg_receiver;
    if (ss.peek() == ' ')
        ss.get(); // consume the space
    std::getline(ss, msg);

    std::cout << "Debug: prefix='" << prefix << "', command='" << command << "', msg_receiver='" << msg_receiver << "', msg='" << msg << "'" << std::endl;

    // check if command is PRIVMSG first
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
    // :<botnic>!<botname>@<bothost> NOTICE <msg_receiver> :<msg>";
    // first change all \n in msg to \r\n
    std::string prefix = ":" + name + "!" + name + "@" + hostname + " NOTICE " + msg_receiver + " :";
    
    std::string finalmessage;
    size_t start = 0, end;
    while ((end = msg.find('\n', start)) != std::string::npos) {
        finalmessage += prefix + msg.substr(start, end - start) + "\r\n";
        start = end + 1;
    }
    finalmessage += prefix + msg.substr(start) + "\r\n";

    return finalmessage;
}

void BotBase::connect_to_server(const std::string &ip, const std::string &port, const std::string &pass)
{
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
    connected = true;

}

void BotBase::disconnect_from_server()
{
    if (connected)
    {
        if (botsocket != -1) {
            close(botsocket);
        }
        botsocket = -1;
        connected = false;
    }
}

void BotBase::run()
{

    while (connected)
    {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds); // Monitor standard input as well
        FD_SET(botsocket, &readfds);
        int max_fd = (botsocket > STDIN_FILENO) ? botsocket : STDIN_FILENO;
        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0 && errno != EINTR) {
            throw BotException("select error");
        }
        if (FD_ISSET(botsocket, &readfds)) {
            char buffer[512];
            ssize_t bytesRead = recv(botsocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0';
                std::string message(buffer);
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
            }
        }
    }

}

int BotBase::getBotFd() const
{
    return botsocket;
}

bool BotBase::isConnected() const
{
    return connected;
}

std::string BotBase::generateAuthMessage(const std::string &pass)
{
    std::string fullstring;
    fullstring = "PASS " + pass + "\r\n";
    fullstring += "NICK " + name + "\r\n";
    fullstring += "USER " + name + " 0 * :" + name + "\r\n";
    return fullstring;
}
