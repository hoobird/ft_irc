#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>

// rationale: fd exceed is caught in socket creation, setting CHANLIMIT is optional, not important

// note: we follow settings using DALnet as default client application
// refer to http://www.irc.org/tech_docs/005.html

enum Limits {
    LIMITS_NETWORK = 0,
    LIMITS_MAXBANS,
    LIMITS_MAXCHANNELS,
    LIMITS_CHANNELLEN,
    LIMITS_KICKLEN,
    LIMITS_NICKLEN,
    LIMITS_TOPICLEN,
    LIMITS_MODES,
    LIMITS_CHANTYPES,
    LIMITS_CHANLIMIT,
    LIMITS_PREFIX,
    LIMITS_STATUSMSG,
    LIMITS_CASEMAPPING,
    LIMITS_CHANMODES,
    LIMITS_TARGMAX,
} ;

inline std::string getLimitString(Limits limit) {
    switch (limit) {
        case LIMITS_NETWORK: return "SAFELIST";
        case LIMITS_MAXBANS: return "200";
        case LIMITS_MAXCHANNELS: return "20";
        case LIMITS_CHANNELLEN: return "32";
        case LIMITS_KICKLEN: return "307";
        case LIMITS_NICKLEN: return "30";
        case LIMITS_TOPICLEN: return "307";
        case LIMITS_MODES: return "6";
        case LIMITS_CHANTYPES: return "#";
        case LIMITS_CHANLIMIT: return "#:20";
        case LIMITS_PREFIX: return "(o)@";
        case LIMITS_STATUSMSG: return "@";
        case LIMITS_CASEMAPPING: return "ascii";
        // note that no flags are in CAT_A
        case LIMITS_CHANMODES: return ",ko,l,it";
        // commands INVITE, NICK, USER, MODE and PASS do not require a max allowed target count
        // limits can be but are not set at startup for the below commands
        case LIMITS_TARGMAX: return "DCCALLOW:,JOIN:,KICK:,PART:,PRIVMSG:";

        default: return "Unable to retrieve limits";
    }
}

enum ChanModes {
    CHANMODES_CAT_A = 0, // change list/address
    CHANMODES_CAT_B, // take params for both "+/-"
    CHANMODES_CAT_C, // only take param for "+"
    CHANMODES_CAT_D, // does not take params
} ;

inline std::string getChanMode(ChanModes mode) {
    switch (mode) {
        case CHANMODES_CAT_A: return "";
        case CHANMODES_CAT_B: return "ko";
        case CHANMODES_CAT_C: return "l";
        case CHANMODES_CAT_D: return "it";
    }
}

#endif
