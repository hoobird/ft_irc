#ifndef NETWORK_ERRORS_HPP
#define NETWORK_ERRORS_HPP

#include <string>

// Error codes for network operations: 100s
enum NetworkError {
    NET_SUCCESS = 0,
    NET_ERR_SOCKET_CREATE,
    NET_ERR_GETADDRINFO,
    NET_ERR_SETSOCKOPT,
    NET_ERR_NONBLOCK_SERVER,
    NET_ERR_BIND,
    NET_ERR_LISTEN,
    NET_ERR_FAIL_TO_FIND_ADDR,
    NET_ERR_EPOLL_CREATE,
    NET_ERR_EPOLL_ADD,
    NET_ERR_EPOLL_EVENT,
    NET_ERR_EPOLL_WAIT,
    NET_ERR_ACCEPT,
    NET_ERR_NONBLOCK_CLIENT, 
    NET_ERR_READ,
    NET_ERR_WRITE,
    NET_ERR_CLOSE,
};

// Helper function to get error string
inline std::string getNetworkErrorString(NetworkError err) {
    switch (err) {
        case NET_SUCCESS: return "Success";
        case NET_ERR_SOCKET_CREATE: return "Failed to create server socket";
        case NET_ERR_GETADDRINFO: return "Failed to get address info";
        case NET_ERR_SETSOCKOPT: return "Failed to set socket options";
        case NET_ERR_NONBLOCK_SERVER: return "Failed to set server socket to non-blocking mode";
        case NET_ERR_BIND: return "Failed to bind server socket";
        case NET_ERR_LISTEN: return "Failed to listen on server socket";
        case NET_ERR_FAIL_TO_FIND_ADDR: return "Failed to find address for server socket";
        case NET_ERR_EPOLL_CREATE: return "Failed to create epoll instance";
        case NET_ERR_EPOLL_ADD: return "Failed to add server socket to epoll";
        case NET_ERR_EPOLL_EVENT: return "Failed to add fd to epoll";
        case NET_ERR_EPOLL_WAIT: return "Failed to wait for epoll events";
        case NET_ERR_ACCEPT: return "Failed to accept new client connection";
        case NET_ERR_NONBLOCK_CLIENT: return "Failed to set client socket to non-blocking mode";
        case NET_ERR_READ: return "Failed to read data from client";
        case NET_ERR_WRITE: return "Failed to write data to client";
        case NET_ERR_CLOSE: return "Failed to close socket";
    
        default: return "Unknown error";
    }
}

#endif
