#include "DataStore.hpp"

// Constructor
DataStore::DataStore() {}

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

// Clients Management
void DataStore::addClient(Client* client) {
    if (client)
        clients[client->getSocketFd()] = client;
}

void DataStore::addClients(const std::vector<Client *> &clients)
{
    for (std::vector<Client *>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
        addClient(*it);
    }
}

void DataStore::removeClient(int clientId) {
    // TODO: to remove from client map, but also Channel map if client is in any channel
    std::cout << "Removing client with ID: " << clientId << " from DataStore" << std::endl;
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

Client *DataStore::getClient(const std::string &nickName) const
{
    for (ClientMap::const_iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second->getNickname() == nickName)
            return it->second;
    }
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

DataStore::ChannelMap::const_iterator DataStore::getChannelsBegin() const
{
    return channels.begin();
}

DataStore::ChannelMap::const_iterator DataStore::getChannelsEnd() const
{
    return channels.end();
}

std::vector<Channel*> DataStore::getChannelsForClient(const Client& client) const {
    std::vector<Channel*> clientChannels;

    for (ChannelMap::const_iterator it = getChannelsBegin(); it != getChannelsEnd(); ++it) {
        if (it->second->isMember(client))
            clientChannels.push_back(it->second);
    }
    return clientChannels;
}

int DataStore::countChannelsForClient(const Client& client) const
{
    std::vector<Channel*> channels = getChannelsForClient(client);
    int count = static_cast<int>(channels.size());
    return count;
}
