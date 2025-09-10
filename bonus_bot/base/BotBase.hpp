#ifndef BOTBASE_HPP
#define BOTBASE_HPP

#include <string>
#include <iostream>

class BotBase {
protected:
    std::string server_ip_;
    int server_port_;
    bool connected_;

public:
    BotBase(const std::string& ip, int port);
    virtual ~BotBase();

    // Pure virtual function - bots must implement this
    virtual void handle_server_message(const std::string& message) = 0;

    // Common functionality
    void connect_to_server();
    void disconnect_from_server();
    void run();

    // Getters
    std::string get_server_ip() const;
    int get_server_port() const;
    bool is_connected() const;
};

#endif