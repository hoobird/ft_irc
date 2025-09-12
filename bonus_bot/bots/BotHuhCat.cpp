#ifndef BOTHUHCAT_CPP
#define BOTHUHCAT_CPP
#include "BotBase.hpp"

class BotHuhCat : public BotBase {
    public:
        BotHuhCat();
        virtual ~BotHuhCat();
        virtual std::string handle_server_message(BotBase::MessageIN messageIN);
        
    private:
        BotHuhCat(const BotHuhCat&);
        BotHuhCat& operator=(const BotHuhCat&);
};
#endif

BotBase *g_bot = NULL;

void handler(int signum) {
    std::cout << "Signal " << signum << " received, shutting down..." << std::endl;
    if (signum == SIGINT || signum == SIGQUIT) {
        if (g_bot) {
            g_bot->shutdown();
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <server_port> <bot_password>" << std::endl;
        return 1;
    }
    std::string server_ip = argv[1];
    std::string server_port = argv[2];
    std::string bot_password = argv[3];
    
    BotHuhCat bot;
    g_bot = &bot;
    if (bot.setupSignalHandler(handler) == false) {
        std::cerr << "Error: Unable to set-up signal handlers." << std::endl;
        return 1;
    }
    try {
        bot.connect_to_server(server_ip, server_port, bot_password);
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

std::string BotHuhCat::handle_server_message(BotBase::MessageIN messageIN)
{
    (void) messageIN;
    std::string response;
    response += " : ?    ?      \n";
    response += " :  ／l、       \n";
    response += " : (ﾟ､ ｡ ７ ?   \n";
    response += " : l、ﾞ ~ヽ     \n";
    response += " :  じしf_, )ノ \n";
    response += " :  MIAU?      \n";
    
    return response;
}
