#include "Server.hpp"

// Server::Server(int port, const std::string &password)
//     :dataStore(port, password), networkMan(&dataStore, &socketFd), cmdHandler(), cmdParser(), running(true) {}
Server::Server(int port, const std::string &password)
    :dataStore(port, password), networkMan(&dataStore, &socketFd), running(true) {}

Server::~Server()
{
}

void Server::start()
{
    NetworkError err = networkMan.setupServerSocket();
    if (err != NET_SUCCESS) {
        std::cerr << "Error: " << getNetworkErrorString(err) << std::endl;
        return;
    }

    err = networkMan.createEpollInstance();
    if (err != NET_SUCCESS) {
        std::cerr << getNetworkErrorString(err) << std::endl;
        return;
    }

    runEventLoop();
}

void Server::shutdown()
{
    running = false;
    std::cout << "Server is shutting down..." << std::endl;
}

void Server::runEventLoop()
{
    while (this->running) {
        NetworkManager::EpollResult events = networkMan.monitorEvents();
        if (events.first < 0) {
            std::cerr << "Error: " << getNetworkErrorString(NET_ERR_EPOLL_WAIT) << std::endl;
            break;
        }
        for (int i = 0; i < events.first; ++i) {
            if (events.second[i].data.fd == socketFd) {
                // Handle new connection
                networkMan.addNewClient();
            } else {
                // Handle existing client
                networkMan.handleClientData(events.second[i].data.fd);
            }
        }
    }
}
