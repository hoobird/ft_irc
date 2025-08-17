#ifndef NETWORKMANAGER_HPP
#define NETWORKMANAGER_HPP

#include <string>
#include <iostream>
#include <NetworkErrors.hpp>
#include "Client.hpp"
#include "MessageBuilder.hpp"
#include "MessageParser.hpp"
#include "DataStore.hpp"
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
		NetworkManager(DataStore *dataStore, int *serverSocketFd);
		~NetworkManager();

		NetworkError setupServerSocket();
		int setNonBlocking(int fd);
		NetworkError createEpollInstance();

		typedef std::pair<int, struct epoll_event*> EpollResult; // trigger count and triggered events
		EpollResult monitorEvents(); 
		
		int addFdToEpoll(int fd);

		void sendResponse(int fd, const std::string& message);
		void addNewClient();
		void handleClientData(int fd);
		void removeClient(int fd);

	private:
		static const int MAX_EVENTS = 512;
		static const int MAX_BUFFER_SIZE = 512; // https://modern.ircdocs.horse/ under Message Format
		DataStore &dataStore;
		int &serverSocketFd;
		struct epoll_event events[MAX_EVENTS];
		int epollFd;

		// OCF
		NetworkManager();
		NetworkManager(const NetworkManager& other);
		NetworkManager& operator=(const NetworkManager& other);
};

#endif