#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>
#include <map>
#include "Client.hpp"

class Channel {
public:
	Channel(const std::string& name);
	~Channel();

	void broadcast(const std::string& message, Client* exclude);
	void setMode(const std::string& mode, const std::string& value);
	void addMember(Client* client);
	void removeMember(Client* client);
	void addOperator(Client* client);
	void removeOperator(Client* client);
	bool isOperator(Client* client) const;

	// getters
	std::string getName() const;
	std::string getTopic() const;
	std::set<int> getMembers() const;
	std::set<int> getOperators() const;
	std::map<std::string, std::string> getModes() const;
	std::string getKey() const;
	int getLimit() const;

	// setters
	void setName(const std::string& name);
	void setTopic(const std::string& topic);
	void setKey(const std::string& key);
	void setLimit(int limit);

private:
	std::string name;
	std::string topic;
	std::set<int> members; // to decide either int or Client*
	std::set<int> operators; // to decide either int or Client*
	std::map<std::string, std::string> modes; // eliz to decide on this if u doing MODE
	std::string key;
	int limit; // 0 means no limit

	// OCF
	Channel();
	Channel(const Channel& other);
	Channel& operator=(const Channel& other);
};

#endif
