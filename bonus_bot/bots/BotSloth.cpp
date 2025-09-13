#ifndef BOTSLOTH_CPP
#define BOTSLOTH_CPP
#include "BotBase.hpp"

class BotSloth : public BotBase {
    public:
        BotSloth();
        virtual ~BotSloth();
        virtual std::pair<std::string,std::string> handle_server_message(BotBase::MessageIN messageIN);

    private:
        BotSloth(const BotSloth&);
        BotSloth& operator=(const BotSloth&);
};
#endif

int main(int argc, char** argv) {
    std::vector<std::string> args = parseArgs(argc, argv);
    if (args.size() != 3) {
        return 1;
    }
    BotSloth bot;
    try {
        bot.connect_to_server(args);
        bot.run();
    } catch (const BotBase::BotException& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

BotSloth::BotSloth(): BotBase() {
    name = "Sloth";
}

BotSloth::~BotSloth(){}

std::pair<std::string,std::string> BotSloth::handle_server_message(BotBase::MessageIN messageIN)
{
    std::string message = messageIN.message;
    std::string response;
    std::string::const_iterator it = message.begin();
    usleep(20000000);
    while (it != message.end()) {
        response += *it;
        response += "   ";
        ++it;
    }

    if (messageIN.receiver[0] == '#') {
        return std::make_pair(messageIN.receiver, response); // respond in channel
    } else {
        return std::make_pair(messageIN.source, response); // respond in private message
    }
}
