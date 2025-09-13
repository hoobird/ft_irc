#ifndef BOTHUHCAT_CPP
#define BOTHUHCAT_CPP
#include "BotBase.hpp"

class BotParrot : public BotBase {
    public:
        BotParrot();
        virtual ~BotParrot();
        virtual std::pair<std::string,std::string> handle_server_message(BotBase::MessageIN messageIN);

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
    name = "HuhCat";
}

BotParrot::~BotParrot(){}

std::pair<std::string,std::string> BotParrot::handle_server_message(BotBase::MessageIN messageIN)
{
    (void) messageIN;
    std::string response;
    response += " : ?    ?      \n";
    response += " :  ／l、       \n";
    response += " : (ﾟ､ ｡ ７ ?   \n";
    response += " : l、ﾞ ~ヽ     \n";
    response += " :  じしf_, )ノ \n";
    response += " :  MIAU?      \n";

    if (messageIN.receiver[0] == '#') {
        return std::make_pair(messageIN.receiver, response); // respond in channel
    } else {
        return std::make_pair(messageIN.source, response); // respond in private message
    }
}
