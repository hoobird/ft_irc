#include "Server.hpp"

Server::Server(int port, const std::string &password)
    : serverName("QI-IRC"), running(true), port(port), password(password), dataStore(), networkMan(), msgParser(), cmdFactory(), cmdHandler(cmdFactory), msgBuilder(serverName)
{}

Server::~Server()
{
}

void Server::start()
{
    NetworkError err = networkMan.setupServerSocket(getPortString());
    if (err != NET_SUCCESS) {
        std::cerr << "Error: " << getNetworkErrorString(err) << std::endl;
        return;
    }

    err = networkMan.createEpollInstance();
    if (err != NET_SUCCESS) {
        std::cerr << getNetworkErrorString(err) << std::endl;
        return;
    }

    setupCommandFactory(); // register commands to factory
    runEventLoop();
}

void Server::shutdown()
{
    running = false;
    std::cout << "Server is shutting down..." << std::endl;
}

void Server::runEventLoop()
{
    while (this->running) {
        // sleepCountdown(); // TODO: for testing only, remove later

        NetworkManager::EpollResult events = networkMan.monitorEvents();
        if (events.first < 0) {
            std::cerr << "Error: " << getNetworkErrorString(NET_ERR_EPOLL_WAIT) << std::endl;
            break;
        }
        for (int i = 0; i < events.first; ++i) {
            int fd = events.second[i].data.fd;
            if (fd == networkMan.getListenerFd()) {
                handleNewConnection();
            } else {
                handleClientEvent(fd);
            }
        }
    }
}

void Server::sleepCountdown()
{
    for (int count = 5; count > 0; --count) {
        std::cout << "Sleeping countdown " << count << "..." << std::endl;
        sleep(1);
    }
}

void Server::setupCommandFactory()
{
    std::cout << "Setting up command factory..." << std::endl;
    // Register commands here
    cmdFactory.registerCommand("USER", new CommandUSER(dataStore));
    cmdFactory.registerCommand("NICK", new CommandNICK(dataStore));
    cmdFactory.registerCommand("PASS", new CommandPASS(*this)); // Pass reference to AuthService (Server)
    cmdFactory.registerCommand("PING", new CommandPING(serverName));
    // cmdFactory.registerCommand("NICK", new NickCommand());
    // cmdFactory.registerCommand("USER", new UserCommand());
    // cmdFactory.registerCommand("JOIN", new JoinCommand());
    // cmdFactory.registerCommand("PART", new PartCommand());
    // cmdFactory.registerCommand("PRIVMSG", new PrivMsgCommand());
    // cmdFactory.registerCommand("QUIT", new QuitCommand());
    // cmdFactory.registerCommand("PASS", new PassCommand());
}

void Server::handleNewConnection()
{
    std::vector<Client *> newClients = networkMan.addNewClients();
    dataStore.addClients(newClients);
}

void Server::handleClientEvent(int fd)
{
    while (true) {
        std::pair<ssize_t, std::string> readResult = networkMan.readClientData(fd);
        ssize_t bytesRead = readResult.first;

        if (bytesRead < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                networkMan.closeConnection(fd);
                dataStore.removeClient(fd);
            } else {
                std::cout << "Finish reading from client fd " << fd << std::endl;
            }
            break;
        }
        if (bytesRead == 0) {
            networkMan.closeConnection(fd);
            dataStore.removeClient(fd);
            break;
        }
        Client &client = *dataStore.getClient(fd);
        std::string bufferString = client.getBuffer() + readResult.second;
        client.clearBuffer();

        processClientMessages(fd, bufferString);
        client.setBuffer(bufferString);
    }
}

void Server::processClientMessages(int clientFd, std::string& bufferString)
{
    size_t pos = bufferString.find("\r\n");
    while (pos != std::string::npos) {
        std::string messageToProcess = bufferString.substr(0, pos + 2);
        bufferString.erase(0, pos + 2);
        std::cout << "Client " << clientFd << " -> Server: " << messageToProcess << std::endl;
        ParsedMessage parsedMessage = msgParser.parse(messageToProcess);
        msgParser.printParsedMessage(parsedMessage);
        Client* client = dataStore.getClient(clientFd);
        if (!client) {
            // this should not happen
            std::cerr << "🚨Warning: Client in epoll queue but not Client Datastore???" << std::endl;
            networkMan.closeConnection(clientFd);
            return;
        }
        responseList rlist = cmdHandler.handleCommand(*client, parsedMessage);
        // printReponseList(rlist); // for debugging
        for (responseList::iterator it = rlist.begin(); it != rlist.end(); ++it) {
            singleResponse response = *it;
            std::string builtMessage = msgBuilder.buildFromTemplate(response);
            std::set<int> fdsToSend = parseClientFds(response["<clientsToSend>"]);
            for (std::set<int>::iterator fdIt = fdsToSend.begin(); fdIt != fdsToSend.end(); ++fdIt) {
                Client* targetClient = dataStore.getClient(*fdIt);
                if (targetClient) {
                    std::cout << "Server -> Client " << *fdIt << ": " << builtMessage << std::endl;
                    networkMan.sendResponse(*fdIt, builtMessage);
                } else {
                    std::cerr << "🚨 Warning: Tried to send message to non-existent client fd " << *fdIt << std::endl;
                }
            }
        }
        // find pos of next \r\n for next iteration
        pos = bufferString.find("\r\n");
    }
}

// Getters
int Server::getPort() const {
    return port;
}

std::string Server::getPortString() const {
    std::stringstream ss;
    ss << port;
    return ss.str();
}

// AuthService interface (only way to access password)

bool Server::validatePassword(const std::string& input) const {
    return input == password; // Simple password validation
}
