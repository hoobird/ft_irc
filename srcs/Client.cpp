#include "Client.hpp"

Client::Client(int fd, std::string hostname)
    : socketfd(fd), hostname(hostname), nickname(""), realname(""), username(""), buffer(""), registered(false), passwordValidated(false)
{
}

Client::~Client()
{
    close(this->socketfd);
}

void Client::clearBuffer()
{
    this->buffer.clear();
}

void Client::setRegistered()
{
    this->registered = true;
}

bool Client::isRegistered() const
{
    return this->registered;
}

bool Client::isReadyToRegister() const
{
    if (this->passwordValidated && !this->nickname.empty() && !this->username.empty() && !this->realname.empty())
        return true;
    return false;
}

void Client::setPasswordValidated()
{
    this->passwordValidated = true;
}

bool Client::isPasswordValidated() const
{
    return this->passwordValidated;
}

int Client::getSocketFd() const
{
    return socketfd;
}

std::string Client::getSocketFdString() const
{
    std::ostringstream ss;
    ss << socketfd;
    return ss.str();
}

std::string Client::getHostname() const
{
    return this->hostname;
}

std::string Client::getNickname() const
{
    return this->nickname;
}

std::string Client::getRealname() const
{
    return  this->realname;
}

std::string Client::getUsername() const
{
    return  this->username;
}

std::string Client::getBuffer() const
{
    return this->buffer;
}

std::string Client::getClientPrefix() const
{
    return this->isRegistered() ? this->nickname : "*";
}

void Client::setHostname(const std::string &hostname)
{
    this->hostname = hostname;
}

void Client::setNickname(const std::string &nick)
{
    this->nickname = nick;
}

void Client::setRealname(const std::string &realname)
{
    this->realname = realname;
}

void Client::setUsername(const std::string &username)
{
    this->username = username;
}

void Client::setBuffer(const std::string& buf)
{
    this->buffer = buf;
}