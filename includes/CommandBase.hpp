#ifndef ACOMMAND_HPP
#define ACOMMAND_HPP

#include <types/ResponseType.hpp>
#include "Client.hpp"
#include "MessageParser.hpp"
#include <set>
#include <sstream>
#include <string>

class CommandBase {
    public:
        virtual ~CommandBase() {}
        virtual responseList execute(Client& client, const ParsedMessage& message) = 0;
		virtual CommandBase* clone() const = 0;

    protected:
        CommandBase() {}
        singleResponse createSingleResponse(const std::string& numeric, const std::string& clientsToSend) const;
        responseList createWelcomeResponse(const Client& client, const std::string serverName) const;
        std::string intSetToCSVString(std::set<int> fdset);

    private:
        CommandBase(const CommandBase&);
        CommandBase& operator=(const CommandBase&);
};

#endif