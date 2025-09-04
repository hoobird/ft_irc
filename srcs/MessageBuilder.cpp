#include "MessageBuilder.hpp"

MessageBuilder::MessageBuilder(const std::string &serverName): serverName(serverName)
{
    initializeNumericTemplates();
}

MessageBuilder::~MessageBuilder()
{
}

std::string MessageBuilder::buildFromTemplate(const singleResponse &responseMap)
{
    // printSingleResponse(responseMap); // for debugging
    std::string numeric;
    try {
        numeric = responseMap.at("<numeric>");
    } catch (const std::out_of_range &e) {
        // NOTE: this should not happen
        std::cerr << "🚨 Developer forgot to add numeric key to your response" << std::endl;
        return "";
    }
    std::map<std::string, std::string>::const_iterator it = numericTemplates.find(numeric);
    if (it == numericTemplates.end()) {
        // NOTE: this should not happen
        std::cerr << "🚨 Warning: Numeric template " << numeric << " not found in Numerics Map" << std::endl;
        return "";
    }

    std::string templateStr = it->second;
    // find all placeholders in the template and extract into vector
    std::set<std::string> placeholderSet;
    std::string::size_type start = 0;
    while (true) {
        // find the next '<'
        start = templateStr.find("<", start);
        if (start == std::string::npos) {
            break;
        }

        // find the closing '>'
        std::string::size_type end = templateStr.find(">", start);
        if (end == std::string::npos) {
            std::cerr << "🚨 Warning: Unmatched '<' in template for numeric " << numeric << "." << std::endl;
            break;
        }

        std::string placeholder = templateStr.substr(start, end - start + 1);
        placeholderSet.insert(placeholder);
        start = end + 1; // move past the '>'
        if (start >= templateStr.size()) {
            // prevent out of bounds access
            break;
        }
    }

    for (std::set<std::string>::const_iterator it = placeholderSet.begin(); it != placeholderSet.end(); ++it) {
        std::string placeholder = *it;
        singleResponse::const_iterator replIt = responseMap.find(placeholder);
        if (replIt == responseMap.end()) {
            std::cerr << "🚨 Warning: Placeholder '" << placeholder << "' not found in replacements for numeric " << numeric << "." << std::endl;
            continue; // skip this placeholder if not found
        }

        std::string replacement = replIt->second;
        std::string::size_type pos = templateStr.find(placeholder);
        if (pos == std::string::npos) {
            std::cerr << "🚨 Warning: Placeholder '" << placeholder << "' not found in template for numeric " << numeric << "." << std::endl;
            continue; // skip if placeholder not found in template
        }
        while (pos != std::string::npos) {
            templateStr.replace(pos, placeholder.length(), replacement);
            pos = templateStr.find(placeholder, pos + replacement.length());
        }
    }

    // prepend prefix for numerics, not command actions
    if (numeric[0] >= '0' && numeric[0] <= '9') {
        templateStr = ":" + serverName + " " + numeric + " " + templateStr;
    }

    return templateStr;
}


void MessageBuilder::initializeNumericTemplates()
{
    std::cout << "Loading numeric templates..." << std::endl;

    // Command Action response
    // for example successfuly NICK, JOIN, PART, QUIT responses are not part of numerics, they are command actions
    numericTemplates["NICK"] = ":<oldnick>!<user>@<host> NICK :<new_nick>";
    numericTemplates["PONG"] = ":<server> PONG <token> :<nick>";
    numericTemplates["PRIVMSG"] = ":<nick_sender>!<user_sender>@<host_sender> PRIVMSG <msg_receiver> :<msg>";
    numericTemplates["JOIN"] = ":<nick_sender>!<user_sender>@<host_sender> JOIN :<channel>";
    numericTemplates["PART"] = ":<nick_sender>!<user_sender>@<host_sender> PART <channel> :<reason>";
    numericTemplates["TOPIC"] = ":<nick_sender>!<user_sender>@<host_sender> TOPIC <channel> :<new_topic>";
    numericTemplates["KICK"] = ":<nick_sender>!<user_sender>@<host_sender> KICK <channel> <target_member> <comment>";

    // Numeric templates for various IRC responses
    // prefix = ":<server> <numeric> "
    numericTemplates["001"] = "<client> :Welcome to the Internet Relay Network <nick>!<user>@<host>"; // RPL_WELCOME
    numericTemplates["002"] = "<client> :Your host is <servername>, running version <version>"; // RPL_YOURHOST
    numericTemplates["003"] = "<client> :This server was created <date>"; // RPL_CREATED
    numericTemplates["004"] = "<client> <server_name> <version> <usermodes> <chanmodes> [chanmodes_with_a_parameter]"; // RPL_MYINFO
    numericTemplates["005"] = "<client> :Try server <server_name>, port <port_number>"; // RPL_BOUNCE
    numericTemplates["200"] = "<client> Link <version>[.<debug_level>] <destination> <next_server> [V<protocol_version> <link_uptime_in_seconds> <backstream_sendq> <upstream_sendq>]"; // RPL_TRACELINK
    numericTemplates["201"] = "<client> Try. <class> <server>"; // RPL_TRACECONNECTING
    numericTemplates["202"] = "<client> H.S. <class> <server>"; // RPL_TRACEHANDSHAKE
    numericTemplates["203"] = "<client> ???? <class> [<connection_address>]"; // RPL_TRACEUNKNOWN
    numericTemplates["204"] = "<client> Oper <class> <nick>"; // RPL_TRACEOPERATOR
    numericTemplates["205"] = "<client> User <class> <nick>"; // RPL_TRACEUSER
    numericTemplates["206"] = "<client> Serv <class> <int>S <int>C <server> <nick!user|*!*>@<host|server> [V<protocol_version>]"; // RPL_TRACESERVER
    numericTemplates["207"] = "<client> Service <class> <name> <type> <active_type>"; // RPL_TRACESERVICE
    numericTemplates["208"] = "<client> <newtype> 0 <client_name>"; // RPL_TRACENEWTYPE
    numericTemplates["209"] = "<client> Class <class> <count>"; // RPL_TRACECLASS
    numericTemplates["211"] = "<client> <linkname> <sendq> <sent_msgs> <sent_bytes> <recvd_msgs> <rcvd_bytes> <time_open>"; // RPL_STATSLINKINFO
    numericTemplates["212"] = "<client> <command> <count> [<byte_count> <remote_count>]"; // RPL_STATSCOMMANDS
    numericTemplates["213"] = "<client> C <host> * <name> <port> <class>"; // RPL_STATSCLINE
    numericTemplates["214"] = "<client> N <host> * <name> <port> <class>"; // RPL_STATSNLINE
    numericTemplates["215"] = "<client> I <host> * <host> <port> <class>"; // RPL_STATSILINE
    numericTemplates["216"] = "<client> K <host> * <username> <port> <class>"; // RPL_STATSKLINE
    numericTemplates["218"] = "<client> Y <class> <ping_freq> <connect_freq> <max_sendq>"; // RPL_STATSYLINE
    numericTemplates["219"] = "<client> <query> :<info>"; // RPL_ENDOFSTATS
    numericTemplates["221"] = "<client> <user_modes> [<user_mode_params>]"; // RPL_UMODEIS
    numericTemplates["234"] = "<client> <name> <server> <mask> <type> <hopcount> <info>"; // RPL_SERVLIST
    numericTemplates["235"] = "<client> <mask> <type> :<info>"; // RPL_SERVLISTEND
    numericTemplates["241"] = "<client> L <hostmask> * <servername> <maxdepth>"; // RPL_STATSLLINE
    numericTemplates["242"] = "<client> :Server Up <days> days <hours>:<minutes>:<seconds>"; // RPL_STATSUPTIME
    numericTemplates["243"] = "<client> O <hostmask> * <opername> [<privileges>] <class>"; // RPL_STATSOLINE
    numericTemplates["244"] = "<client> H <hostmask> * <servername>"; // RPL_STATSHLINE
    numericTemplates["251"] = "<client> :There are <int> users and <int> invisible on <int> servers"; // RPL_LUSERCLIENT
    numericTemplates["252"] = "<client> <int> :operator(s) online"; // RPL_LUSEROP
    numericTemplates["253"] = "<client> <int> :unknown connection(s)"; // RPL_LUSERUNKNOWN
    numericTemplates["254"] = "<client> <int> :channels formed"; // RPL_LUSERCHANNELS
    numericTemplates["255"] = "<client> :I have <int> clients and <int> servers"; // RPL_LUSERME
    numericTemplates["256"] = "<client> <server> :Administrative info"; // RPL_ADMINME
    numericTemplates["257"] = "<client> :<admin_location>"; // RPL_ADMINLOC1
    numericTemplates["258"] = "<client> :<admin_location>"; // RPL_ADMINLOC2
    numericTemplates["259"] = "<client> :<email_address>"; // RPL_ADMINEMAIL
    numericTemplates["261"] = "<client> File <logfile> <debug_level>"; // RPL_TRACELOG
    numericTemplates["262"] = "<client> <server_name> <version>[.<debug_level>] :<info>"; // RPL_TRACEEND
    numericTemplates["263"] = "<client> <command> :Please wait a while and try again."; // RPL_TRYAGAIN
    numericTemplates["301"] = "<client> <nick> :<message>"; // RPL_AWAY
    numericTemplates["302"] = "<client> :*1<reply> *( ' ' <reply> )"; // RPL_USERHOST
    numericTemplates["303"] = "<client> :*1<nick> *( ' ' <nick> )"; // RPL_ISON
    numericTemplates["305"] = "<client> :<info>"; // RPL_UNAWAY
    numericTemplates["306"] = "<client> :<info>"; // RPL_NOWAWAY
    numericTemplates["311"] = "<client> <nick> <user> <host> * :<real_name>"; // RPL_WHOISUSER
    numericTemplates["312"] = "<client> <nick> <server> :<server_info>"; // RPL_WHOISSERVER
    numericTemplates["313"] = "<client> <nick> :<privileges>"; // RPL_WHOISOPERATOR
    numericTemplates["314"] = "<client> <nick> <user> <host> * :<real_name>"; // RPL_WHOWASUSER
    numericTemplates["315"] = "<client> <name> :<info>"; // RPL_ENDOFWHO
    numericTemplates["317"] = "<client> <nick> <seconds> :seconds idle"; // RPL_WHOISIDLE
    numericTemplates["318"] = "<client> <nick> :<info>"; // RPL_ENDOFWHOIS
    numericTemplates["319"] = "<client> <nick> :*( ( '@' / '+' ) <channel> ' ' )"; // RPL_WHOISCHANNELS
    numericTemplates["321"] = "<client> Channels :Users Name"; // RPL_LISTSTART
    numericTemplates["322"] = "<client> <channel> <#_visible> :<topic>"; // RPL_LIST
    numericTemplates["323"] = "<client> :<info>"; // RPL_LISTEND
    numericTemplates["324"] = "<client> <channel> <mode> <mode_params>"; // RPL_CHANNELMODEIS
    numericTemplates["325"] = "<client> <channel> <nickname>"; // RPL_UNIQOPIS
    numericTemplates["331"] = "<client> <channel> :<info>"; // RPL_NOTOPIC
    numericTemplates["332"] = "<client> <channel> :<topic>"; // RPL_TOPIC
    numericTemplates["341"] = "<client> <nick> <channel>"; // RPL_INVITING
    numericTemplates["342"] = "<client> <user> :<info>"; // RPL_SUMMONING
    numericTemplates["346"] = "<client> <channel> <invitemask>"; // RPL_INVITELIST
    numericTemplates["347"] = "<client> <channel> :<info>"; // RPL_ENDOFINVITELIST
    numericTemplates["348"] = "<client> <channel> <exceptionmask> [<who> <set-ts>]"; // RPL_EXCEPTLIST
    numericTemplates["349"] = "<client> <channel> :<info>"; // RPL_ENDOFEXCEPTLIST
    numericTemplates["351"] = "<client> <version> <server> :<comments>"; // RPL_VERSION
    numericTemplates["352"] = "<client> <channel> <user> <host> <server> <nick> <H|G>[*][@|+] :<hopcount> <real_name>"; // RPL_WHOREPLY
    // numericTemplates["353"] = "<client> ( '=' / '*' / '@' ) <channel> ' ' : [ '@' / '+' ] <nick> *( ' ' [ '@' / '+' ] <nick> )"; // RPL_NAMREPLY
    numericTemplates["353"] = "<client> = <channel> :<nicks>"; // RPL_NAMREPLY
    numericTemplates["364"] = "<client> <mask> <server> :<hopcount> <server_info>"; // RPL_LINKS
    numericTemplates["365"] = "<client> <mask> :<info>"; // RPL_ENDOFLINKS
    numericTemplates["366"] = "<client> <channel> :<info>"; // RPL_ENDOFNAMES
    numericTemplates["367"] = "<client> <channel> <banid> [[<setter> <time_left>|<time_left> :<reason>]]"; // RPL_BANLIST
    numericTemplates["368"] = "<client> <channel> :<info>"; // RPL_ENDOFBANLIST
    numericTemplates["369"] = "<client> <nick> :<info>"; // RPL_ENDOFWHOWAS
    numericTemplates["371"] = "<client> :<string>"; // RPL_INFO
    numericTemplates["372"] = "<client> :- <string>"; // RPL_MOTD
    numericTemplates["374"] = "<client> :<info>"; // RPL_ENDOFINFO
    numericTemplates["375"] = "<client> :- <server> Message of the day -"; // RPL_MOTDSTART
    numericTemplates["376"] = "<client> :<info>"; // RPL_ENDOFMOTD
    numericTemplates["381"] = "<client> :<info>"; // RPL_YOUREOPER
    numericTemplates["382"] = "<client> <config_file> :<info>"; // RPL_REHASHING
    numericTemplates["383"] = "<client> :You are service <service_name>"; // RPL_YOURESERVICE
    numericTemplates["391"] = "<client> <server> :<time string>"; // RPL_TIME
    numericTemplates["392"] = "<client> :UserID Terminal Host"; // RPL_USERSSTART
    numericTemplates["393"] = "<client> :<username> <ttyline> <hostname>"; // RPL_USERS
    numericTemplates["394"] = "<client> :<info>"; // RPL_ENDOFUSERS
    numericTemplates["395"] = "<client> :<info>"; // RPL_NOUSERS
    numericTemplates["401"] = "<client> <nick> :<reason>"; // ERR_NOSUCHNICK
    numericTemplates["402"] = "<client> <server> :<reason>"; // ERR_NOSUCHSERVER
    numericTemplates["403"] = "<client> <channel> :<reason>"; // ERR_NOSUCHCHANNEL
    numericTemplates["404"] = "<client> <channel> :<reason>"; // ERR_CANNOTSENDTOCHAN
    numericTemplates["405"] = "<client> <channel> :<reason>"; // ERR_TOOMANYCHANNELS
    numericTemplates["406"] = "<client> <nick> :<reason>"; // ERR_WASNOSUCHNICK
    numericTemplates["407"] = "<client> <target> :<reason>"; // ERR_TOOMANYTARGETS
    numericTemplates["408"] = "<client> <service_name> :<reason>"; // ERR_NOSUCHSERVICE
    numericTemplates["409"] = "<client> :<reason>"; // ERR_NOORIGIN
    numericTemplates["411"] = "<client> :<reason>"; // ERR_NORECIPIENT
    numericTemplates["412"] = "<client> :<reason>"; // ERR_NOTEXTTOSEND
    numericTemplates["413"] = "<client> <mask> :<reason>"; // ERR_NOTOPLEVEL
    numericTemplates["414"] = "<client> <mask> :<reason>"; // ERR_WILDTOPLEVEL
    numericTemplates["415"] = "<client> <mask> :<reason>"; // ERR_BADMASK
    numericTemplates["421"] = "<client> <command> :<reason>"; // ERR_UNKNOWNCOMMAND
    numericTemplates["422"] = "<client> :<reason>"; // ERR_NOMOTD
    numericTemplates["423"] = "<client> <server> :<reason>"; // ERR_NOADMININFO
    numericTemplates["424"] = "<client> :<reason>"; // ERR_FILEERROR
    numericTemplates["431"] = "<client> :<reason>"; // ERR_NONICKNAMEGIVEN
    numericTemplates["432"] = "<client> <nick> :<reason>"; // ERR_ERRONEUSNICKNAME
    numericTemplates["433"] = "<client> <nick> :<reason>"; // ERR_NICKNAMEINUSE
    numericTemplates["436"] = "<nick> :<reason>"; // ERR_NICKCOLLISION
    numericTemplates["437"] = "<client> <nick/channel/service> :<reason>"; // ERR_UNAVAILRESOURCE
    numericTemplates["441"] = "<client> <nick> <channel> :<reason>"; // ERR_USERNOTINCHANNEL
    numericTemplates["442"] = "<client> <channel> :<reason>"; // ERR_NOTONCHANNEL
    numericTemplates["443"] = "<client> <nick> <channel> [:<reason>]"; // ERR_USERONCHANNEL
    numericTemplates["444"] = "<client> <user> :<reason>"; // ERR_NOLOGIN
    numericTemplates["445"] = "<client> :<reason>"; // ERR_SUMMONDISABLED
    numericTemplates["446"] = "<client> :<reason>"; // ERR_USERSDISABLED
    numericTemplates["451"] = "<client> :<reason>"; // ERR_NOTREGISTERED
    numericTemplates["461"] = "<client> <command> :<reason>"; // ERR_NEEDMOREPARAMS
    numericTemplates["462"] = "<client> :<reason>"; // ERR_ALREADYREGISTERED
    numericTemplates["463"] = "<client> :<reason>"; // ERR_NOPERMFORHOST
    numericTemplates["464"] = "<client> :<reason>"; // ERR_PASSWDMISMATCH
    numericTemplates["465"] = "<client> :<reason>"; // ERR_YOUREBANNEDCREEP
    numericTemplates["467"] = "<client> <channel> :<reason>"; // ERR_KEYSET
    numericTemplates["471"] = "<client> <channel> :<reason>"; // ERR_CHANNELISFULL
    numericTemplates["472"] = "<client> <char> :<reason>"; // ERR_UNKNOWNMODE
    numericTemplates["473"] = "<client> <channel> :<reason>"; // ERR_INVITEONLYCHAN
    numericTemplates["474"] = "<client> <channel> :<reason>"; // ERR_BANNEDFROMCHAN
    numericTemplates["475"] = "<client> <channel> :<reason>"; // ERR_BADCHANNELKEY
    numericTemplates["476"] = "<client> <channel> :<reason>"; // ERR_BADCHANMASK
    numericTemplates["477"] = "<client> <channel> :<reason>"; // ERR_NOCHANMODES
    numericTemplates["478"] = "<client> <channel> [char] :<reason>"; // ERR_BANLISTFULL
    numericTemplates["481"] = "<client> :<reason>"; // ERR_NOPRIVILEGES
    numericTemplates["482"] = "<client> <channel> :<reason>"; // ERR_CHANOPRIVSNEEDED
    numericTemplates["483"] = "<client> :<reason>"; // ERR_CANTKILLSERVER
    numericTemplates["484"] = "<client> :<reason>"; // ERR_RESTRICTED
    numericTemplates["485"] = "<client> :<reason>"; // ERR_UNIQOPRIVSNEEDED
    numericTemplates["491"] = "<client> :<reason>"; // ERR_NOOPERHOST
    numericTemplates["501"] = "<client> :<reason>"; // ERR_UMODEUNKNOWNFLAG
    numericTemplates["502"] = "<client> :<reason>"; // ERR_USERSDONTMATCH
}
