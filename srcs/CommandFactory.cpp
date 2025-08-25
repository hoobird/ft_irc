#include "CommandFactory.hpp"

CommandFactory::CommandFactory()
{
}

CommandFactory::~CommandFactory()
{
    for (std::map<std::string, CommandBase *>::iterator it = commandMap.begin(); it != commandMap.end(); ++it)
    {
        delete it->second;
    }
}

void CommandFactory::registerCommand(const std::string &commandName, CommandBase *command)
{
    commandMap[commandName] = command;
}

CommandBase *CommandFactory::cloneCommand(const std::string &commandName)
{
    std::map<std::string, CommandBase *>::iterator it = commandMap.find(commandName);
    if (it != commandMap.end())
    {
        return it->second->clone();
    }
    return NULL; // command not found
}
