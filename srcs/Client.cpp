#include "Client.hpp"

Client::Client(int fd, std::string hostname)
    : socketfd(fd), hostname(hostname), nickname(""), fullname(""), username(""), buffer(""), registered(false)
{
}

Client::~Client()
{
}

void Client::clearBuffer()
{
    this->buffer.clear();
}

int Client::getSocketFd() const
{
    return socketfd;
}

std::string Client::getBuffer() const
{
    return this->buffer;
}

void Client::setBuffer(const std::string& buf)
{
    this->buffer = buf;
}