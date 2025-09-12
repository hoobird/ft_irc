#ifndef COMMANDMODE_HPP
#define COMMANDMODE_HPP

#include "CommandBase.hpp"
#include "Client.hpp"
#include "DataStore.hpp"
#include <vector>
#include <string>

class CommandMODE : public CommandBase {
public:
    // typedef std::vector<std::pair<std::string, std::string> > vecModeFlags; // vector base struct that stores the mode flags
	// i.e.
		// '-i', "";
		// '-i', "";
		// '+i', "";
		// '-i', "";
		// '-t', "";
		// '+t', "";
		// '+t', "";
		// '+t', "";
		// '-k', "pass";
		// '+k', "pass";
		// '+k', "pass";
		// '+k', "pass";
		// '-o', "userNick";
		// '+o', "userNick";
		// '+o', "userNick";
		// '-l', "";
		// '+l', "1";
		// '+l', "5";
		// '+l', "10";
    typedef std::map<char, std::vector<std::pair<char, std::string> > > mapModeFlags; // map base struct that stores the mode flags
	// i.e.
		// i:
			// '-', "";
			// '-', "";
			// '+', "";
			// '-', "";
		// t:
			// '-', "";
			// '+', "";
			// '+', "";
			// '+', "";
		// k:
			// '-', "pass";
			// '+', "pass";
			// '+', "pass";
			// '+', "pass";
		// o:
			// '-', "userNick";
			// '+', "userNick";
			// '+', "userNick";
		// l:
			// '-', "";
			// '+', "1";
			// '+', "5";
			// '+', "10";

    CommandMODE(DataStore const & dataStore);
    CommandMODE(const CommandMODE& other);
    ~CommandMODE();
    responseList errorHandle(const ParsedMessage& message, const std::string clientFdStr, const std::string clientNick);
    bool needParameter(char commandType, char operation);
    mapModeFlags parse(const ParsedMessage& message);
    void handleSingleCallModes(); // reconsider refactoring
    void handleMultiCallModes(); // reconsider refactoring

    responseList execute(Client& client, const ParsedMessage& message);
	CommandBase* clone() const;

private:
    DataStore const &dataStore;
    // OCF
    CommandMODE();
    CommandMODE& operator=(const CommandMODE& other);
};

#endif
