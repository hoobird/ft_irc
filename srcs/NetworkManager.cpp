#include "NetworkManager.hpp"

NetworkManager::NetworkManager(): listenerFd(-1), epollFd(-1) {}

NetworkManager::~NetworkManager() {
    // Close the epoll instance and server socket and clients sockets
    if (epollFd >= 0) {
        close(epollFd);
    }
    if (listenerFd >= 0) {
        close(listenerFd);
    }
    for (std::set<int>::iterator it = epollClientFds.begin(); it != epollClientFds.end(); ++it) {
        close(*it);
    }
}

NetworkError NetworkManager::setupServerSocket(const std::string &portString)
{
    std::cout << "Setting up server socket..." << std::endl;

    struct addrinfo hints, *servinfo, *p;
    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE; // Fill in my IP for me

    int status = getaddrinfo(NULL, portString.c_str(), &hints, &servinfo);
    if (status != 0) {
        return NET_ERR_GETADDRINFO;
    }
    int socketFd  = -1;
    int count = 0;
    // Loop through all the results and bind to the first we can
    std::cout << "Binding to available address..." << std::endl;
    std::cout << "Note: errors might appear but they are controlled, until we find a valid address." << std::endl;
    for (p = servinfo; p != NULL; p = p->ai_next) {
        ++count;
        // Create socket
        socketFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (socketFd < 0) {
            std::cerr << "Error: " << getNetworkErrorString(NET_ERR_SOCKET_CREATE) << " (Try: " << count << ")" << std::endl;
            continue;
        }

        // Set socket options to reuse address
        int opt = 1;
        if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            std::cerr << "Error(Reuse address): " << getNetworkErrorString(NET_ERR_SETSOCKOPT) << " (Try: " << count << ")" << std::endl;
            close(socketFd);
            continue;
        }

        // IPv6 to support IPv4 connections also (Dual Stack)
        if (p->ai_family == AF_INET6) {
            int ipv6only = 0;
            if (setsockopt(socketFd, IPPROTO_IPV6, IPV6_V6ONLY, &ipv6only, sizeof(ipv6only)) < 0) {
                std::cerr << "Error(IPV6_V6ONLY): " << getNetworkErrorString(NET_ERR_SETSOCKOPT) << " (Try: " << count << ")" << std::endl;
                close(socketFd);
                continue;
            }
        }

        // Make socket non-blocking
        if (setNonBlocking(socketFd) != 0) {
            std::cerr << "Error: " << getNetworkErrorString(NET_ERR_NONBLOCK_CLIENT) << " (Try: " << count << ")" << std::endl;
            close(socketFd);
            continue;
        }

        // Bind the socket
        if (bind(socketFd, p->ai_addr, p->ai_addrlen) < 0) {
            std::cerr << "Error: " << getNetworkErrorString(NET_ERR_BIND) << " (Try: " << count << ")" << std::endl;
            close(socketFd);
            continue;
        }

        // Listen on the socket
        if (listen(socketFd, SOMAXCONN) < 0) {
            std::cerr << "Error: " << getNetworkErrorString(NET_ERR_LISTEN) << " (Try: " << count << ")" << std::endl;
            close(socketFd);
            continue;
        }
        break;
    }

    if (p == NULL) {
        freeaddrinfo(servinfo);
        return NET_ERR_FAIL_TO_FIND_ADDR;
    }
    // Successfully bound to an address
    this->listenerFd = socketFd;
    std::cout << "Server socket successfully set up on port " << portString << std::endl;
    freeaddrinfo(servinfo); // Free the linked list
    return NET_SUCCESS;
}

int NetworkManager::setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
    {
       return -1;
    }
    return 0;
}

NetworkError NetworkManager::createEpollInstance()
{
    std::cout << "Creating epoll instance..." << std::endl;
    this->epollFd = epoll_create1(0);
    if (this->epollFd < 0)
    {
        return NET_ERR_EPOLL_CREATE;
    }
    // Add the server socket to epoll
    if (addFdToEpoll(this->listenerFd) != 0)
    {
        return NET_ERR_EPOLL_ADD;
    }

    return NET_SUCCESS;
}

int NetworkManager::addFdToEpoll(int fd)
{
    std::cout << "Adding fd " << fd << " to epoll" << std::endl;
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(this->epollFd, EPOLL_CTL_ADD, fd, &event) < 0)
    {
        return -1;
    }
    return 0;
}

NetworkManager::EpollResult NetworkManager::monitorEvents() {
    std::cout << "\nMonitoring events...\n" << std::endl;
    int triggerCount = epoll_wait(this->epollFd, this->events, MAX_EVENTS, -1);
    if (triggerCount < 0) {
        return std::make_pair(triggerCount, static_cast<epoll_event*>(NULL));
    }
    return std::make_pair(triggerCount, this->events);
}

std::vector<Client*> NetworkManager::addNewClients() {
    std::vector<Client*> newClients;
    while (true)  {
        std::cout << "Adding new client..." << std::endl;
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientFd = accept(this->listenerFd, (struct sockaddr *)&clientAddr, &clientLen);
        if (clientFd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // we processed all of the incoming new connections
                break;
            }
            std::cerr << "Error: " << getNetworkErrorString(NET_ERR_ACCEPT) << std::endl;
            continue;
        }
        // Make the client socket non-blocking
        if (setNonBlocking(clientFd) != 0) {
            std::cerr << "Error: " << getNetworkErrorString(NET_ERR_NONBLOCK_CLIENT) << std::endl;
            close(clientFd);
            continue;
        }
        // Add the client socket to epoll
        if (addFdToEpoll(clientFd) != 0) {
            std::cerr << "Error: " << getNetworkErrorString(NET_ERR_EPOLL_ADD) << std::endl;
            close(clientFd);
            continue;
        }
        // Create Client object
        // By right should be Server's job, but we do it here for simplicity
        // if not we have to return vector of pairs, each pair is clientfd and client address
        Client *newClient = new Client(clientFd, inet_ntoa(clientAddr.sin_addr));
        newClients.push_back(newClient);
        epollClientFds.insert(clientFd); // track client fds added to epoll
    }
    return newClients;
}

std::pair<ssize_t, std::string> NetworkManager::readClientData(int fd) {
    std::cout << "Reading existing client data from fd " << fd << std::endl;
    char buffer[MAX_BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0);
    std::string bufferString = std::string(buffer, bytesRead > 0 ? bytesRead : 0);
    return std::make_pair(bytesRead, bufferString);
}

void NetworkManager::closeConnection(int fd)
{
    if (epoll_ctl(this->epollFd, EPOLL_CTL_DEL, fd, NULL) < 0) {
        std::cerr << "Error: " << getNetworkErrorString(NET_ERR_EPOLL_EVENT_DEL) << std::endl;
    }
    if (close(fd) < 0) {
        std::cerr << "Error: " << getNetworkErrorString(NET_ERR_CLOSE) << std::endl;
    } else {
        std::cout << "Closed connection for fd " << fd << std::endl;
    }
    epollClientFds.erase(fd); // remove from tracked client fds
}

// return 0 on success, 1 if the caller should retry later, -1 on error and close the connection
int NetworkManager::sendResponse(int fd, const std::string& message) {
    // add \r\n to the end of the message
    std::string response = message + "\r\n";
    errno = 0; // Reset errno before send
    ssize_t bytesSent = send(fd, response.c_str(), response.size(), MSG_NOSIGNAL);
    if (bytesSent < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Try again later (e.g., when EPOLLOUT is triggered)
            return 1; // Indicate to caller to retry
        }
        if (errno == EPIPE) {
            std::cerr << "Error: " << getNetworkErrorString(NET_ERR_SEND_EPIPE) << std::endl;
        } else {
            std::cerr << "Error: " << getNetworkErrorString(NET_ERR_SEND) << " (Broken pipe or connection reset)" << std::endl;
        }
        closeConnection(fd);
        return -1;
    }
    std::cout << "Sent " << bytesSent << " bytes to fd " << fd << std::endl;
    return 0;
}

int NetworkManager::getListenerFd() const
{
    return this->listenerFd;
}
