#ifndef COMMANDMODE_HPP
#define COMMANDMODE_HPP

#include "CommandBase.hpp"
#include "Client.hpp"
#include "DataStore.hpp"
#include <vector>
#include <string>

class CommandMODE : public CommandBase {
public:
    typedef std::map<char, std::vector<std::pair<char, std::string> > > modeFlags; // struct that stores the mode flags
    CommandMODE(DataStore const & dataStore);
    CommandMODE(const CommandMODE& other);
    ~CommandMODE();
    responseList errorHandle(const ParsedMessage& message, const std::string clientFdStr, const std::string clientNick);
    responseList execute(Client& client, const ParsedMessage& message);
    modeFlags parse(const ParsedMessage& message);
	CommandBase* clone() const;

private:
    DataStore const &dataStore;
    // OCF
    CommandMODE();
    CommandMODE& operator=(const CommandMODE& other);
};

#endif
