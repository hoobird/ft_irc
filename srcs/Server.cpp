#include "Server.hpp"

Server::Server(int port, const std::string &password, Logger& logger)
    :serverName("IRCh"), running(true), port(port), password(password), dataStore(),
    networkMan(), msgParser(), cmdFactory(), cmdHandler(cmdFactory), msgBuilder(serverName), logger(logger)
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
        // sleepCountdown(5); // TODO: for testing only, remove later

        NetworkManager::EpollResult events = networkMan.monitorEvents();
        if (events.first < 0) {
            if (running == true) { // only warn if server did not shutdown
                std::cerr << "Warning: " << getNetworkErrorString(NET_ERR_EPOLL_WAIT) << std::endl;
            }
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

void Server::sleepCountdown(int seconds)
{
    for (int count = seconds; count > 0; --count) {
        std::cout << "Sleeping countdown " << count << "..." << std::endl;
        sleep(1);
    }
}

void Server::setupCommandFactory()
{
    std::cout << "Setting up command factory..." << std::endl;
    // Register commands here
    cmdFactory.registerCommand("USER", new CommandUSER(dataStore, serverName));
    cmdFactory.registerCommand("NICK", new CommandNICK(dataStore, serverName));
    cmdFactory.registerCommand("PASS", new CommandPASS(*this, serverName)); // Pass reference to AuthService (Server)
    cmdFactory.registerCommand("PING", new CommandPING(serverName));
    cmdFactory.registerCommand("MODE", new CommandMODE(dataStore));
    cmdFactory.registerCommand("PRIVMSG", new CommandPRIVMSG(dataStore));
    cmdFactory.registerCommand("JOIN", new CommandJOIN(dataStore));
    cmdFactory.registerCommand("PART", new CommandPART(dataStore));
    cmdFactory.registerCommand("TOPIC", new CommandTOPIC(dataStore));
    cmdFactory.registerCommand("KICK", new CommandKICK(dataStore));
    cmdFactory.registerCommand("INVITE", new CommandINVITE(dataStore));
    cmdFactory.registerCommand("QUIT", new CommandQUIT(dataStore, networkMan));
    cmdFactory.registerCommand("NOTICE", new CommandNOTICE(dataStore));
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
        std::cout << "Client Buffer: " << client.getBuffer() << std::endl;
        std::cout << "Incoming data: " << readResult.second << std::endl;

        client.clearBuffer();

        processClientMessages(client, bufferString);
        if (dataStore.getClient(fd) == NULL) // prevent setBuffer when client no longer exists (commandQUIT)
            break ;
        client.setBuffer(bufferString);
    }
}

void Server::processClientMessages(Client& client, std::string& bufferString)
{
    size_t pos = bufferString.find("\r\n");
    while (pos != std::string::npos) {
        std::string messageToProcess = bufferString.substr(0, pos); // dont include \r\n
        bufferString.erase(0, pos + 2); // remove processed message and \r\n from buffer
        if (client.isPrevBufferOverflow() || messageToProcess.size() > 510) { // to exclude \r\n
            // how to move on to next batch of string to process?
            pos = bufferString.find("\r\n");
            client.setPrevBufferOverflow(false);
            continue;
        }

        std::cout << "(Client " << client.getSocketFdString() << " ➡️  Server  )\t" << messageToProcess << std::endl;
        logger << "(Client " << client.getSocketFdString() << " ➡️ Server  )\t" << messageToProcess << std::endl;

        ParsedMessage parsedMessage = msgParser.parse(messageToProcess);
        msgParser.printParsedMessage(parsedMessage);

        responseList rlist = cmdHandler.handleCommand(client, parsedMessage);
        // printReponseList(rlist); // for debugging
        for (responseList::iterator it = rlist.begin(); it != rlist.end(); ++it) {
            singleResponse response = *it;
            std::string builtMessage = msgBuilder.buildFromTemplate(response);
            if (builtMessage.empty())
                continue;
            std::set<int> fdsToSend = parseClientFds(response["<clientsToSend>"]);
            for (std::set<int>::iterator fdIt = fdsToSend.begin(); fdIt != fdsToSend.end(); ++fdIt) {
                Client* targetClient = dataStore.getClient(*fdIt);
                if (targetClient) {
                    std::cout << "(Server   ➡️  Client " << *fdIt << ")\t" << builtMessage << std::endl;
                    logger << "(Server   ➡️ Client " << *fdIt << ")\t" << builtMessage << std::endl;
                    networkMan.sendResponse(*fdIt, builtMessage);
                } else {
                    std::cerr << "🚨 Warning: Tried to send message to non-existent client fd " << *fdIt << std::endl;
                }
            }
        }
        // find pos of next \r\n for next iteration
        pos = bufferString.find("\r\n");
    }
    // If cannot find \r\n and string is already > 510,
    //  not possible for str to be processed into valid command
    if (bufferString.size() > 510) {
        client.setPrevBufferOverflow(true);
        bufferString.clear();
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
