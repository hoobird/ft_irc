#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>
#include <map>
#include <ctime> // for Channel Creation time and Topic update time
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

	void addMember(Client& client);
	void removeMember(Client& client);
	bool isMember(const Client& client) const;
	void addOperator(Client& client);
	void removeOperator(Client& client);
	bool isOperator(const Client& client) const;
    bool isMemberInvited(const Client& client) const;
	void addInvitedUser(const Client& client); // for commandINVITE not commandMODE +i
    void removeInvitedUser(const Client& client); // for commandJOIN, to remove invited user upon successful channel join
    // commandMODE i: inviteList always exists, just whether there are invited users called through commandINVITE
	std::string displayModes() const;

	// getters
	time_t getCreationTime() const;
	std::string getCreationTimeString() const;
	time_t getTopicUpdateTime() const;
	std::string getTopicUpdateTimeString() const;
	std::string getName() const;
	std::string getTopic() const;
	std::string getTopicAuthor() const;
	std::set<int> getMembers() const;
	std::set<int> getOperators() const;
	std::set<int> getInviteList() const;
	std::map<std::string, std::string> getModes() const;
	std::string getKey() const;
	int getLimit() const;
    bool getInviteMode() const;
    bool getTopicRestrict() const;

	// setters
	void setTopicUpdateTime(); // no input cuz the time is now
	void setName(const std::string& name);
	void setTopic(const std::string& topic);
	void setTopicAuthor(const std::string& author);
	void setKey(const std::string& key);
	void setLimit(int limit);
    void setInviteMode(bool status);
    void setTopicRestrict(bool status);

private:
	time_t creationTime;
	time_t topicUpdateTime; // 0 if unset
	std::string name;
	std::string topic;
	std::string topicAuthor;
	std::set<int> members;
	std::set<int> operators;
    std::set<int> inviteList; // tracks which client has been invited but yet to join the channel
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
