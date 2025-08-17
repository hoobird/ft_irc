#include "NetworkManager.hpp"

NetworkManager::NetworkManager(DataStore *dataStore, int *serverSocketFd)
    :dataStore(*dataStore), serverSocketFd(*serverSocketFd), epollFd(-1) {
}

NetworkManager::~NetworkManager() {
    // Close the epoll instance and server socket and clients sockets
    if (epollFd >= 0) {
        close(epollFd);
    }
    if (serverSocketFd >= 0) {
        close(serverSocketFd);
    }
    for (DataStore::ClientMap::const_iterator it = dataStore.getClientsBegin(); it != dataStore.getClientsEnd(); ++it) {
        close(it->first); // Close client socket
        // destruction of Client objects will be handled by DataStore destructor
    }
}

NetworkError NetworkManager::setupServerSocket()
{
    std::cout << "Setting up server socket..." << std::endl;

    struct addrinfo hints, *servinfo, *p;
    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE; // Fill in my IP for me

    int status = getaddrinfo(NULL, dataStore.getPortString().c_str(), &hints, &servinfo);
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
        return NET_ERR_FAIL_TO_FIND_ADDR;
    }
    // Successfully bound to an address
    this->serverSocketFd = socketFd;
    std::cout << "Server socket successfully set up on port " << dataStore.getPortString() << std::endl;
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
    if (addFdToEpoll(this->serverSocketFd) != 0)
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
    std::cout << "Monitoring events..." << std::endl;
    int triggerCount = epoll_wait(this->epollFd, this->events, MAX_EVENTS, -1);
    if (triggerCount < 0) {
        return std::make_pair(triggerCount, static_cast<epoll_event*>(NULL));
    }
    return std::make_pair(triggerCount, this->events);
}

void NetworkManager::sendResponse(int fd, const std::string& message) {
    std::cout << "Sending response to fd " << fd << ": " << message << std::endl;
    // TODO: Send message to client
}

void NetworkManager::addNewClient() {
    while (true)  {
        std::cout << "Adding new client..." << std::endl;
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientFd = accept(this->serverSocketFd, (struct sockaddr *)&clientAddr, &clientLen);
        if (clientFd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // we processed all of the incoming new connections
                return;
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
        // Create Client object and add to DataStore
        Client *newClient = new Client(clientFd, inet_ntoa(clientAddr.sin_addr));
        dataStore.addClient(newClient);
        std::cout << "New client added with fd " << clientFd << std::endl;
    }
}

void NetworkManager::handleClientData(int fd) {
    std::cout << "Reading existing client data from fd " << fd << std::endl;
    char buffer[MAX_BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            // finish reading data from client
        }
        else
        {
            // Handle read error close client fd and remove from DataStore
            if (close(fd) < 0) {
                std::cerr << "Error: " << getNetworkErrorString(NET_ERR_CLOSE) << std::endl;
            } else {
                std::cout << "Closed client fd " << fd << std::endl;
            }
            dataStore.removeClient(fd); // Remove client from DataStore
        }
        return;
    }
    if (bytesRead == 0) {
        // Client disconnected, remove client
        // Similar to above, need to remove client from DataStore
        if (close(fd) < 0) {
            std::cerr << "Error: " << getNetworkErrorString(NET_ERR_CLOSE) << std::endl;
        } else {
            std::cout << "Closed client fd " << fd << std::endl;
        }
        dataStore.removeClient(fd); // Remove client from DataStore
        return;
    } 
    // bytesRead > 0
    Client &client = *dataStore.getClient(fd);
    std::string bufferString = client.getBuffer() + std::string(buffer);
    client.clearBuffer();

    // cannot find \r\n in the buffer, so we put back to buffer for next recv
    size_t pos = bufferString.find("\r\n");
    if (pos == std::string::npos) {
        client.setBuffer(bufferString);
        return;
    }
    // Process the buffer
    std::string messageToProcess = bufferString.substr(0, pos + 2); // Include \r\n
    std::string remainingBuffer = bufferString.substr(pos + 2);
    if (!remainingBuffer.empty()) {
        client.setBuffer(remainingBuffer);
    }

    std::cout << "Client " << fd << " -> Server: " << messageToProcess << std::endl;
}

void NetworkManager::removeClient(int fd) {
    std::cout << "Removing client with fd " << fd << std::endl;
    // TODO: Remove client from clients map
}
