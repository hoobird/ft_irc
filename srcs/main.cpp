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


static bool serverRunning = true; // Control epoll loop

// Port must be between 1024 and 65535 as ports below 1024 are reserved
const int MIN_PORT = 1024;
const int MAX_PORT = 65535;

void    signalHandler(int signum) {
    if (signum == SIGINT || signum == SIGQUIT) {
        // Handle Ctrl+C signal and Ctrl+\ signal
        serverRunning = false;
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

bool checkInputArgs(int argc, char** argv, std::string& port, std::string& password) {
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
    long portNum = std::strtol(port.c_str(), &endptr, base);
    if (errno == ERANGE || portNum < 0 || portNum > std::numeric_limits<int>::max() || *endptr != '\0') {
        std::cerr << "Error: Invalid port number." << std::endl;
        return false;
    }
    if (portNum <= MIN_PORT || portNum > MAX_PORT) 
    {
        std::cerr << "Error: Port number must be between 1024 and 65535." << std::endl;
        return false;
    }

    return true;
}

int main(int argc, char** argv) {
    
    std::string port;
    std::string password;

    // Register signal handlers
    if (!setupSignalHandlers()) {
        return -1;
    }

    // add a case that will fail cppcheck
    int numbers[] = {1, 2, 3, 4, 5};
    std::cout << "the 6th number is: " << numbers[5] << std::endl; // This will cause an out-of-bounds access

    // valgrind leak
    std::string* leakString = new std::string("This is a memory leak example");
    (void)leakString; // Suppress unused variable warning, but this will leak memory

    // Check input arguments
    if (!checkInputArgs(argc, argv, port, password)) {
        return -1;
    }

    // while loop that just runs some waiting logic
    while (serverRunning) {
        // Simulate waiting logic
        std::cout << "Waiting..." << std::endl;
        sleep(1); // Sleep for 1 second
    }


    return 0;
}