#ifndef CONFIG_HPP
#define CONFIG_HPP


// rationale: fd exceed is caught in socket creation, setting CHANLIMIT is optional, not important

struct LIMITS
{
    static const int CLIENT_MAX_CHANNEL = 10;
    static const int CHANNEL_MAX_CLIENT = 50; // TODO: Look into server's client limit.
} ;

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