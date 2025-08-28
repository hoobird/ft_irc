#ifndef NETWORKMANAGER_HPP
#define NETWORKMANAGER_HPP

#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <NetworkErrors.hpp>
#include "Client.hpp"
#include "MessageBuilder.hpp"
#include "MessageParser.hpp"
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <cerrno>

class NetworkManager {
	public:
		NetworkManager();
		~NetworkManager();

		NetworkError setupServerSocket(const std::string &portString);
		int setNonBlocking(int fd);
		NetworkError createEpollInstance();

		typedef std::pair<int, struct epoll_event*> EpollResult; // trigger count and triggered events
		EpollResult monitorEvents();

		int addFdToEpoll(int fd);

		std::vector<Client*> addNewClients();
		std::pair<ssize_t, std::string> readClientData(int fd); // resturns bytes read and buffer from recv()
		void closeConnection(int fd);
		int sendResponse(int fd, const std::string& message);

		// Getters
		int getListenerFd() const;

	private:
		static const int MAX_EVENTS = 512;
		static const int MAX_BUFFER_SIZE = 512; // https://modern.ircdocs.horse/ under Message Format
		int listenerFd;
		int epollFd;
		struct epoll_event events[MAX_EVENTS];
		std::set<int> epollClientFds; // to keep track of  all client fds added to epoll

		// OCF
		NetworkManager(const NetworkManager& other);
		NetworkManager& operator=(const NetworkManager& other);
};

#endif