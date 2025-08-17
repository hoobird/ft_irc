#include "DataStore.hpp"

// Constructor
DataStore::DataStore(int port, const std::string& password)
    : port(port), password(password) {}

// Destructor
DataStore::~DataStore() {
    // Clean up clients
    for (ClientMap::iterator it = clients.begin(); it != clients.end(); ++it) {
        delete it->second;
    }
    clients.clear();

    // Clean up channels
    for (ChannelMap::iterator it = channels.begin(); it != channels.end(); ++it) {
        delete it->second;
    }
    channels.clear();
}

// Getters
int DataStore::getPort() const {
    return port;
}

std::string DataStore::getPortString() const {
    std::stringstream ss;
    ss << port;
    return ss.str();
}

const std::string& DataStore::getPassword() const {
    return password;
}

// Clients Management
void DataStore::addClient(Client* client) {
    if (client)
        clients[client->getSocketFd()] = client;
}

void DataStore::removeClient(int clientId) {
    // TODO: to remove from client map, but also Channel map if client is in any channel
    std::cout << "Removing client with ID: " << clientId << "from DataStore" << std::endl;
    ClientMap::iterator it = clients.find(clientId);
    if (it != clients.end()) {
        delete it->second;
        clients.erase(it);
    }
}

Client* DataStore::getClient(int clientId) const {
    ClientMap::const_iterator it = clients.find(clientId);
    if (it != clients.end())
        return it->second;
    return NULL;
}

DataStore::ClientMap::const_iterator DataStore::getClientsBegin() const
{
    return clients.begin();
}

DataStore::ClientMap::const_iterator DataStore::getClientsEnd() const
{
    return clients.end();
}

// Channels Management
void DataStore::addChannel(Channel* channel) {
    if (channel)
        channels[channel->getName()] = channel;
}

void DataStore::removeChannel(const std::string& channelName) {
    ChannelMap::iterator it = channels.find(channelName);
    if (it != channels.end()) {
        delete it->second;
        channels.erase(it);
    }
}

Channel* DataStore::getChannel(const std::string& channelName) const {
    ChannelMap::const_iterator it = channels.find(channelName);
    if (it != channels.end())
        return it->second;
    return NULL;
}
