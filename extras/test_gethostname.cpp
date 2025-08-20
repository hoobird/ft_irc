#include <iostream>
#include <string>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

std::string getHostnameWithGethostname() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        return std::string(hostname);
    }
    return "unknown";
}

std::string getHostnameWithGethostbyname() {
    struct hostent* host = gethostbyname("");
    if (host && host->h_name) {
        return std::string(host->h_name);
    }
    return "unknown";
}

std::string getIPWithGetaddrinfo() {
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    
    struct addrinfo hints = {}, *result;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    if (getaddrinfo(hostname, NULL, &hints, &result) == 0) {
        for (struct addrinfo* addr = result; addr; addr = addr->ai_next) {
            if (addr->ai_family == AF_INET) {
                struct sockaddr_in* ipv4 = (struct sockaddr_in*)addr->ai_addr;
                char ip_str[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &ipv4->sin_addr, ip_str, sizeof(ip_str));
                freeaddrinfo(result);
                return std::string(ip_str);
            }
        }
        freeaddrinfo(result);
    }
    return "127.0.0.1";
}

int main() {
    std::cout << "=== Testing Hostname Methods ===" << std::endl;
    
    std::string hostname1 = getHostnameWithGethostname();
    std::cout << "gethostname() result: " << hostname1 << std::endl;
    
    std::string hostname2 = getHostnameWithGethostbyname();
    std::cout << "gethostbyname(\"\") result: " << hostname2 << std::endl;
    
    std::string ip = getIPWithGetaddrinfo();
    std::cout << "IP address: " << ip << std::endl;
    
    std::cout << "\n=== Testing gethostbyname with different inputs ===" << std::endl;
    
    const char* test_hosts[] = {"", "localhost", "127.0.0.1", NULL};
    
    for (int i = 0; test_hosts[i]; i++) {
        struct hostent* host = gethostbyname(test_hosts[i]);
        if (host) {
            std::cout << "gethostbyname(\"" << test_hosts[i] << "\"): " << host->h_name << std::endl;
            
            // Print all IP addresses
            for (int j = 0; host->h_addr_list[j]; j++) {
                char ip_str[INET_ADDRSTRLEN];
                inet_ntop(host->h_addrtype, host->h_addr_list[j], ip_str, sizeof(ip_str));
                std::cout << "  IP " << j + 1 << ": " << ip_str << std::endl;
            }
        } else {
            std::cout << "gethostbyname(\"" << test_hosts[i] << "\") failed" << std::endl;
        }
        std::cout << std::endl;
    }
    
    return 0;
}