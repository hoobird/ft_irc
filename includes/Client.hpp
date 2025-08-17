#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
public:
	Client(int fd, std::string hostname);
	~Client();

	void send(const std::string& message);
	void clearBuffer();
	bool isRegistered() const;

	// getters
	int getSocketFd() const;
	std::string getHostname() const;
	std::string getNickname() const;
	std::string getFullname() const;
	std::string getUsername() const;
	std::string getBuffer() const;

    // setters
    void setHostname(const std::string& hostname);
    void setNickname(const std::string& nick);
    void setFullname(const std::string& fullname);
    void setUsername(const std::string& username);
    void setBuffer(const std::string& buf);

    

private:
	int socketfd;
	std::string hostname;
	std::string nickname;
	std::string fullname; // Double check if this is needed
	std::string username;
	std::string buffer;
	bool registered;

	// OCF
	Client();
	Client(const Client& other);
	Client& operator=(const Client& other);
};

#endif
