#include "Server.hpp"
#include <iostream>
#include <sstream>
#include <signal.h>
#include <unistd.h>
#include <string>
#include <cstdlib>
#include <cerrno>
#include <limits>
#include <fcntl.h>

Server* g_server = NULL; // Global pointer to Server instance

// Port must be between 1024 and 65535 as ports below 1024 are reserved
const int MIN_PORT = 1024;
const int MAX_PORT = 65535;

void    signalHandler(int signum) {
    if (signum == SIGINT || signum == SIGQUIT) {
        // Handle Ctrl+C signal and Ctrl+\ signal
        g_server->shutdown();
    }
}

bool    setupSignalHandlers() {
    struct sigaction sa;
    sa.sa_handler = signalHandler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) != 0 || 
        sigaction(SIGQUIT, &sa, NULL) != 0) {
        std::cerr << "Error: Unable to set-up signal handlers." << std::endl;
        return false;
    }
    return true;
}

bool checkInputArgs(int argc, char** argv, std::string &port, int &portNum, std::string &password) {
    if (argc != 3) {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return false;
    }
    
    port = std::string(argv[1]);
    password = std::string(argv[2]);

    if (port.empty() || password.empty()) {
        std::cerr << "Error: Port or Password cannot be empty." << std::endl;
        return false;
    }

    // Validate port
    errno = 0;
    char *endptr;
    int base = 10; // Base 10 for decimal
    long portNumLong = std::strtol(port.c_str(), &endptr, base);
    if (errno == ERANGE || portNumLong < 0 || portNumLong > std::numeric_limits<int>::max() || *endptr != '\0') {
        std::cerr << "Error: Invalid port number." << std::endl;
        return false;
    }
    if (portNumLong <= MIN_PORT || portNumLong > MAX_PORT) 
    {
        std::cerr << "Error: Port number must be between 1024 and 65535." << std::endl;
        return false;
    }
    portNum = static_cast<int>(portNumLong);

    return true;
}

int main(int argc, char** argv) {
    
    std::string port;
    int portNum;
    std::string password;

    // Register signal handlers
    if (!setupSignalHandlers()) {
        return -1;
    }

    // Check input arguments and set portNum to integer value
    if (!checkInputArgs(argc, argv, port, portNum, password)) {
        return -1;
    }

    // Create Server instance
    Server server(portNum, password);
    g_server = &server; // Set global pointer to the server instance
    
    server.start();


    return 0;
}