#ifndef ACOMMAND_HPP
#define ACOMMAND_HPP

#include <types/ResponseType.hpp>
#include "Client.hpp"
#include "MessageParser.hpp"

class CommandBase {
    public:
        virtual ~CommandBase() {}
        virtual responseList execute(Client& client, const ParsedMessage& message) = 0;
		virtual CommandBase* clone() const = 0;

    protected:
        CommandBase() {}
        singleResponse createSingleResponse(const std::string& numeric, const std::string& clientsToSend) const;
        responseList createWelcomeResponse(const Client& client) const;

        template<typename T>
        static std::string clientsToFdsString(const T& clients) {
            std::string result;
            for (typename T::const_iterator it = clients.begin(); it != clients.end(); ++it) {
                if (it != clients.begin()) {
                    result += ",";
                }
                result += it->getSocketFdString();
            }
            return result;
        }

    private:
        CommandBase(const CommandBase&);
        CommandBase& operator=(const CommandBase&);
};

#endif