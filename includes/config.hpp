#ifndef CONFIG_HPP
#define CONFIG_HPP

struct limits
{
    static const int CLIENT_MAX_CHANNEL = 10;
    static const int CHANNEL_MAX_CLIENT = 50; // noticed that there is a limit set in Channel private member, ask if that is what it's for.
    

} ;


#endif