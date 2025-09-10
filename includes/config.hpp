#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>

// rationale: fd exceed is caught in socket creation, setting CHANLIMIT is optional, not important

// note: We follow settings using DALnet as default client application
// refer to http://www.irc.org/tech_docs/005.html

struct LIMITS
{
    static const int CLIENT_MAX_CHANNEL = 10;
    static const int CHANNEL_MAX_CLIENT = 50;

    // TODO: Look into server's client limit.
    static const std::string NETWORK;
    static const int MAXBANS = 200;
    static const int MAXCHANNELS = 20;
    static const int CHANNELLEN = 32;
    static const int KICKLEN = 307;
    static const int NICKLEN = 30;
    static const int TOPICLEN = 307;
    static const int MODES = 6;
    static const std::string CHANTYPES;
    static const std::string CHANLIMIT;
    static const std::string PREFIX;
    static const std::string STATUSMSG;

    static const std::string CASEMAPPING;

    // note that no flags are in CAT_A
    static const std::string CHANMODES;

    // commands INVITE, NICK, USER, MODE and PASS do not require a max allowed target count
    // limits can be but are not set at startup for the below commands
    static const std::string TARGMAX;
} ;

const std::string LIMITS::NETWORK = "SAFELIST";
const std::string LIMITS::CHANTYPES = "#";
const std::string LIMITS::CHANLIMIT = "#:20";
const std::string LIMITS::PREFIX = "(o)@";
const std::string LIMITS::STATUSMSG = "@";
const std::string LIMITS::CASEMAPPING = "ascii";
const std::string LIMITS::CHANMODES = ",ko,l,it";
const std::string LIMITS::TARGMAX = "DCCALLOW:,JOIN:,KICK:,PART:,PRIVMSG:";

struct CHANMODES
{
    static const std::string CAT_A; // change list/address
    static const std::string CAT_B; // take params for both "+/-"
    static const std::string CAT_C; // only take param for "+"
    static const std::string CAT_D; // does not take params
} ;

const std::string CHANMODES::CAT_A = "";
const std::string CHANMODES::CAT_B = "ko";
const std::string CHANMODES::CAT_C = "l";
const std::string CHANMODES::CAT_D = "it";

//look into enum for CHANMODES

#endif
