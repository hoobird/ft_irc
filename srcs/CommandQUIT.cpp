#include "AllCommands.hpp"

CommandQUIT::CommandQUIT(DataStore& dataStore, NetworkManager& networkManager)
    : CommandBase(), dataStore(dataStore), networkManager(networkManager)
{}

CommandQUIT::CommandQUIT(const CommandQUIT& other)
    : CommandBase(), dataStore(other.dataStore), networkManager(other.networkManager)
{}

CommandQUIT::~CommandQUIT() {}

responseList CommandQUIT::execute(Client& client, const ParsedMessage& message) {
    responseList responses;

    std::vector<Channel*> channels = dataStore.getChannelsForClient(client);
    std::set<int> channelMates; // to store unique member fds to notify

    // collect all members to notify except for quitting client
    for (std::vector<Channel*>::const_iterator it = channels.begin(); it != channels.end(); ++it) {
        // remove client from channel object (remove from member list and operator list (if applicable))
        (*it)->removeMember(client);
        // add all member fds of channel to channelMates to inform them of action
        std::set<int> members = (*it)->getMembers();
        channelMates.insert(members.begin(), members.end());
        // if channel object has no members left
        if ((*it)->getMembers().empty()) {
            // delete channel object and remove from DataStore ChannelMap (use datastore method)
            this->dataStore.removeChannel((*it)->getName());
        }
    }

    // send QUIT notification to all members in memberFds with reason
    // note: reason is optional, if not provided use ":Client Quit"
    if (!channelMates.empty()) {
        std::string memberFds = intSetToCSVString(channelMates);
        singleResponse resp = createSingleResponse("QUIT", memberFds);
        resp["<nick_sender>"] = client.getClientPrefix();
        resp["<user_sender>"] = client.getUsername();
        resp["<host_sender>"] = client.getHostname();
        if (message.trailing.empty())
            resp["<reason>"] = "Client Quit";
        else
            resp["<reason>"] = message.trailing;
        responses.push_back(resp);
    }

    // close client connection using networkmanager method (closeConnection); let the server close connection instead
    this->networkManager.closeConnection(client.getSocketFd());
    // remove client from dataStore method
    this->dataStore.removeClient(client.getSocketFd());

    return responses;
}

CommandBase* CommandQUIT::clone() const {
    return new CommandQUIT(*this);
}
