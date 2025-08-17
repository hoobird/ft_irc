#include "Channel.hpp"

Channel::Channel(const std::string &name)
    : name(name), topic(""), key(""), limit(0)
{
}

Channel::~Channel() {
    // Destructor logic if needed
}

std::string Channel::getName() const
{
    return this->name;
}
