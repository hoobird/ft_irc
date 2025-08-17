#ifndef DATASTORE_HPP
#define DATASTORE_HPP

#include "Client.hpp"
#include "Channel.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <map>

class DataStore {
    public:
        // Constructor
        DataStore(int port, const std::string& password);

        // Getters
        int getPort() const;
        std::string getPortString() const;
        const std::string& getPassword() const;
        
        typedef std::map<int, Client*> ClientMap;
        // Clients Management
        void addClient(Client* client);
        void removeClient(int clientId);
        Client* getClient(int clientId) const; // NULL if not found
        ClientMap::const_iterator getClientsBegin() const;
        ClientMap::const_iterator getClientsEnd() const;
        
        typedef std::map<std::string, Channel*> ChannelMap;
        // Channels Management
        void addChannel(Channel* channel);
        void removeChannel(const std::string& channelName);
        Channel* getChannel(const std::string& channelName) const; // NULL if not found
        ChannelMap::const_iterator getChannelsBegin() const;
        ChannelMap::const_iterator getChannelsEnd() const;
        
        
        // Destructor
        ~DataStore();
    

    private:
    
        // Server
        int port;
        std::string password;
        
        // Clients and Channels storage
        DataStore();
        ClientMap clients; // Client ID -> Client object 
        ChannelMap channels; // Channel name -> Channel object

        // Useless OCF
        DataStore(const DataStore& other); // Copy constructor
        DataStore& operator=(const DataStore& other); // Assignment operator

};
#endif
