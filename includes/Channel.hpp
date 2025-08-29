#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>
#include <map>
#include "Client.hpp"

// Is client/user invited/banned from the channel
enum Status {
    NONE = 0,
    INVITED,
    BANNED
} ;

class Channel {
public:
	Channel(const std::string& name);
	~Channel();

	void setMode(const std::string& mode, const std::string& value);
	void addMember(Client& client);
	void removeMember(Client& client);
	bool isMember(const Client& client) const;
	void addOperator(Client& client);
	void removeOperator(Client& client);
	bool isOperator(const Client& client) const;
    bool isMemberInvited(const Client& client) const;

	// getters
	std::string getName() const;
	std::string getTopic() const;
	std::set<int> getMembers() const;
	std::set<int> getOperators() const;
	std::set<int> getInviteList() const;
	std::map<std::string, std::string> getModes() const;
	std::string getKey() const;
	int getLimit() const;
    bool getInviteMode() const;
    bool getTopicRestrict() const;

	// setters
	void setName(const std::string& name);
	void setTopic(const std::string& topic);
	void setKey(const std::string& key);
	void setLimit(int limit);
    void setInviteMode(bool status);
    void setTopicRestrict(bool status);

private:
	std::string name;
	std::string topic;
	std::set<int> members;
	std::set<int> operators;
    std::set<int> inviteList; // turn off inviteList when bool value is false
	bool inviteMode;
    bool topicRestrict;
    std::string key; // empty string means no password
	int limit; // -1 means no limit

	// OCF
	Channel();
	Channel(const Channel& other);
	Channel& operator=(const Channel& other);
};

#endif
