#ifndef CTCPHANDLER_HPP
#define CTCPHANDLER_HPP

#include "types/ResponseType.hpp"
#include "types/ParsedMessage.hpp"
#include "Client.hpp"

// Server is a dumb pipe for CTCP commands, just forwards the CTCP messages
// CTCP command handling is done by the clients themselves

class CTCPHandler {
    public:
        CTCPHandler();
        ~CTCPHandler();

        static  bool isCTCPCommand(const ParsedMessage& message);
        responseList handleCTCPCommand(Client& sender, Client &receiver, const ParsedMessage& message);

    private:
        // useless OCF
        CTCPHandler(const CTCPHandler& other);
        CTCPHandler& operator=(const CTCPHandler& other);
};

#endif