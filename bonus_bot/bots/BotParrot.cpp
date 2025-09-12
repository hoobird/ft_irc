#ifndef BOTPARROT_CPP
#define BOTPARROT_CPP
#include "BotBase.hpp"

class BotParrot : public BotBase {
    public:
        BotParrot();
        virtual ~BotParrot();
        virtual std::string handle_server_message(BotBase::MessageIN messageIN);

    private:
        BotParrot(const BotParrot&);
        BotParrot& operator=(const BotParrot&);
};
#endif

int main(int argc, char** argv) {
    std::vector<std::string> args = parseArgs(argc, argv);
    if (args.size() != 3) {
        return 1;
    }
    BotParrot bot;
    try {
        bot.connect_to_server(args);
        bot.run();
    } catch (const BotBase::BotException& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

BotParrot::BotParrot(): BotBase() {
    name = "Parrot";
}

BotParrot::~BotParrot(){}

std::string BotParrot::handle_server_message(BotBase::MessageIN messageIN)
{
    std::string response;
    response += messageIN.message;
    return response;
}
