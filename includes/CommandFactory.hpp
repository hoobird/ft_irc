#ifndef COMMANDFACTORY_HPP
#define COMMANDFACTORY_HPP

#include "CommandBase.hpp"
#include <map>

class CommandFactory {
public:
	CommandFactory();
	~CommandFactory();

	void registerCommand(const std::string& commandName, CommandBase* command);
	CommandBase* cloneCommand(const std::string& commandName);
	
private:
	std::map<std::string, CommandBase*> commandMap;

	// OCF
	CommandFactory(const CommandFactory& other);
	CommandFactory& operator=(const CommandFactory& other);
};

#endif