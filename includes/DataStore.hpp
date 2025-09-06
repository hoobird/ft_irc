#ifndef DATASTORE_HPP
#define DATASTORE_HPP

#include "Client.hpp"
#include "Channel.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

class DataStore {
    public:
        // Constructor
        DataStore();

        typedef std::map<int, Client*> ClientMap;
        // Clients Management
        void addClient(Client* client);
        void addClients(const std::vector<Client*>& clients);
        void removeClient(int clientId);
        Client* getClient(int clientId) const; // NULL if not found
        Client* getClient(const std::string& nickName) const; // NULL if not found
        ClientMap::const_iterator getClientsBegin() const;
        ClientMap::const_iterator getClientsEnd() const;

        typedef std::map<std::string, Channel*> ChannelMap;
        // Channels Management
        void addChannel(Channel* channel);
        void removeChannel(const std::string& channelName);
        Channel* getChannel(const std::string& channelName) const; // NULL if not found
        ChannelMap::const_iterator getChannelsBegin() const;
        ChannelMap::const_iterator getChannelsEnd() const;

        int countChannelsForClient(const Client& client) const;
        std::vector<Channel*> getChannelsForClient(const Client& client) const;

        // Destructor
        ~DataStore();


    private:

        // Clients and Channels storage
        ClientMap clients; // Client ID -> Client object
        ChannelMap channels; // Channel name -> Channel object

        // Useless OCF
        DataStore(const DataStore& other); // Copy constructor
        DataStore& operator=(const DataStore& other); // Assignment operator

};
#endif
