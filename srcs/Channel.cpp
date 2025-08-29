#include "Channel.hpp"

Channel::Channel(const std::string &name)
    : name(name), topic(""), key(""), limit(0), inviteMode(false), topicRestrict(false)
{
}

Channel::~Channel() {
    // Destructor logic if needed
}

void Channel::addMember(Client& client)
{
    if (this->members.empty())
        operators.insert(client.getSocketFd());
    members.insert(client.getSocketFd());
}

void Channel::removeMember(Client& client)
{
    members.erase(client.getSocketFd());
}

bool Channel::isMember(const Client& client) const
{
    return members.find(client.getSocketFd()) != members.end();
}

void Channel::addOperator(Client& client)
{
    operators.insert(client.getSocketFd());
}

void Channel::removeOperator(Client& client)
{
    operators.erase(client.getSocketFd());
}

bool Channel::isOperator(const Client& client) const
{
    return operators.find(client.getSocketFd()) != operators.end();
}

bool Channel::isMemberInvited(const Client& client) const
{
    return inviteList.find(client.getSocketFd()) != inviteList.end();
}

std::string Channel::getName() const
{
    return this->name;
}

std::string Channel::getTopic() const
{
    return this->topic;
}

std::set<int> Channel::getMembers() const
{
    return this->members;
}

std::set<int> Channel::getOperators() const
{
    return this->operators;
}

std::set<int> Channel::getInviteList() const
{
    return this->inviteList;
}

std::string Channel::getKey() const
{
    return this->key;
}

int Channel::getLimit() const
{
    return this->limit;
}

bool Channel::getInviteMode() const
{
    return this->inviteMode;
}

bool Channel::getTopicRestrict() const
{
    return this->topicRestrict;
}

void Channel::setName(const std::string &name)
{
    this->name = name;
}

void Channel::setTopic(const std::string &topic)
{
    this->topic = topic;
}

void Channel::setKey(const std::string &key)
{
    this->key = key;
}

void Channel::setLimit(int limit)
{
    this->limit = limit;
}


void Channel::setInviteMode(bool status)
{
    this->inviteMode = status;
}

void Channel::setTopicRestrict(bool status)
{
    this->topicRestrict = status;
}

