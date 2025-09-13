#ifndef BOTHUHCAT_CPP
#define BOTHUHCAT_CPP
#include "BotBase.hpp"

class BotHuhCat : public BotBase {
    public:
        BotHuhCat();
        virtual ~BotHuhCat();
        virtual std::pair<std::string,std::string> handle_server_message(BotBase::MessageIN messageIN);

    private:
        BotHuhCat(const BotHuhCat&);
        BotHuhCat& operator=(const BotHuhCat&);
};
#endif

int main(int argc, char** argv) {
    std::vector<std::string> args = parseArgs(argc, argv);
    if (args.size() != 3) {
        return 1;
    }
    BotHuhCat bot;
    try {
        bot.connect_to_server(args);
        bot.run();
    } catch (const BotBase::BotException& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

BotHuhCat::BotHuhCat(): BotBase() {
    name = "HuhCat";
}

BotHuhCat::~BotHuhCat(){}

std::pair<std::string,std::string> BotHuhCat::handle_server_message(BotBase::MessageIN messageIN)
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
