#include "AllCommands.hpp"

CommandQUIT::CommandQUIT(DataStore& dataStore, NetworkManager& networkManager)
    : CommandBase(), dataStore(dataStore) , networkManager(networkManager)
{}

CommandQUIT::CommandQUIT(const CommandQUIT& other)
    : CommandBase(), dataStore(other.dataStore), networkManager(other.networkManager)
{}

CommandQUIT::~CommandQUIT() {}

responseList CommandQUIT::execute(Client& client, const ParsedMessage& message) {
    responseList responses;

    std::vector<Channel*> channels = dataStore.getChannelsForClient(client);
    std::set<int> channelmates; // to store unique member fds to notify

    // for each channel in channels:
    //     remove client from channel object (remove from member list and operator list if applicable)
    //     add all of member fds of channel to channelmates to inform them
    //     if channel object has no members left:
    //         delete channel object and remove from DataStore ChannelMap (use datastore method)

    // send QUIT notification to all members in membersFds with reason
    // note: reason is optional, if not provided use ":Client Quit"

    // remove client from server's clients map using datastore method
    // close client connection using networkmanager method (closeConnection)

    return responses
}

CommandBase* CommandQUIT::clone() const {
    return new CommandQUIT(*this);
}
