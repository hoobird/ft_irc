#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <unistd.h>

class Client {
	public:
		Client(int fd, std::string hostname);
		~Client();

		void clearBuffer();
		void setRegistered();
		bool isRegistered() const;
		bool isReadyToRegister() const;
		void setPasswordValidated();
		bool isPasswordValidated() const;

		// getters
		int getSocketFd() const;
		std::string getSocketFdString() const;
		std::string getHostname() const;
		std::string getNickname() const;
		std::string getRealname() const;
		std::string getUsername() const;
		std::string getBuffer() const;
		bool isPrevBufferOverflow() const;

		std::string getClientPrefix() const; // for singleResponse

		// setters
		void setHostname(const std::string& hostname);
		void setNickname(const std::string& nick);
		void setRealname(const std::string& realname);
		void setUsername(const std::string& username);
		void setBuffer(const std::string& buf);
		void setPrevBufferOverflow(bool status);

	private:
		int socketfd;
		std::string hostname;
		std::string nickname;
		std::string realname; // Double check if this is needed
		std::string username;
		std::string buffer;
		bool prevBufferOverflow;
		bool registered;
		bool passwordValidated;

		// OCF
		Client();
		Client(const Client& other);
		Client& operator=(const Client& other);
};

#endif
