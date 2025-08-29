#ifndef CONFIG_HPP
#define CONFIG_HPP


// rationale: fd exceed is caught in socket creation, setting CHANLIMIT is optional, not important

struct limits
{
    static const int CLIENT_MAX_CHANNEL = 10;
    static const int CHANNEL_MAX_CLIENT = 50; // TODO: Look into server's client limit.


} ;


#endif