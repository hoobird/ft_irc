#ifndef BOTHUHCAT_CPP
#define BOTHUHCAT_CPP
#include "BotBase.hpp"

class BotHuhCat : public BotBase {
    public:
        BotHuhCat(): BotBase(){
            name = "HuhCat";
        }
        virtual ~BotHuhCat() {}
        virtual std::string handle_server_message(BotBase::MessageIN messageIN) {
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

};
#endif


int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <server_port> <bot_password>" << std::endl;
        return 1;
    }
    std::string server_ip = argv[1];
    std::string server_port = argv[2];
    std::string bot_password = argv[3];

    BotHuhCat bot;
    try {
        bot.connect_to_server(server_ip, server_port, bot_password);
        bot.run();
    } catch (const BotBase::BotException& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}