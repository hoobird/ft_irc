#include "CommandBase.hpp"

singleResponse CommandBase::createSingleResponse(const std::string &numeric, const std::string &clientsToSend) const
{
    singleResponse resp;
    resp["<numeric>"] = numeric;
    resp["<clientsToSend>"] = clientsToSend;
    return resp;
}

responseList CommandBase::createWelcomeResponse(const Client &client, const std::string serverName) const
{
    responseList responses;
    // responses.reserve(5);
    // Numeric 001 to 005
    // TODO: 005 RPL_ISUPPORT might be needed?
    singleResponse resp = createSingleResponse("001", client.getSocketFdString());
    resp["<client>"] = client.getClientPrefix();
    resp["<nick>"] = client.getNickname();
    resp["<user>"] = client.getUsername();
    resp["<host>"] = client.getHostname();
    responses.push_back(resp);

    resp = createSingleResponse("002", client.getSocketFdString());
    resp["<client>"] = client.getClientPrefix();
    resp["<servername>"] = serverName;
    resp["<version>"] = "1.0";
    responses.push_back(resp);

    resp = createSingleResponse("003", client.getSocketFdString());
    resp["<client>"] = client.getClientPrefix();
    resp["<date>"] = "2025-08-23";
    responses.push_back(resp);

    resp = createSingleResponse("004", client.getSocketFdString());
    resp["<client>"] = client.getClientPrefix();
    resp["<server_name>"] = serverName;
    resp["<version>"] = "1.0";
    resp["<usermodes>"] = "";
    resp["<chanmodes>"] = "";
    responses.push_back(resp);

    // numericTemplates["005"] = "<client> :Try server <server_name>, port <port_number>"; // RPL_BOUNCE
    // resp = createSingleResponse("005", client.getSocketFdString());
    // resp["<client>"] = client.getClientPrefix();
    // resp["<server_name>"] = serverName;
    // resp["<port_number>"] = ??;
    // responses.push_back(resp);

    // RPL_MOTDSTART
    resp = createSingleResponse("375", client.getSocketFdString());
    resp["<client>"] = client.getClientPrefix();
    resp["<server>"] = serverName;
    resp["<info>"] = "Message of the Day -";
    responses.push_back(resp);

    // RPL_MOTD
    resp = createSingleResponse("372", client.getSocketFdString());
    resp["<client>"] = client.getClientPrefix();
    resp["<string>"] = "╭─────────────────────── Welcome To ────────────────────────╮";
    responses.push_back(resp);
    resp["<string>"] = "";
    responses.push_back(resp);
    resp["<string>"] = " 8888888  8888888b.    .d8888b.   888         ╭─┬─╭─╭─╮╮";
    responses.push_back(resp);
    resp["<string>"] = "   888    888   Y88b  d88P  Y88b  888         │ │ ├▓┼▓┤│";
    responses.push_back(resp);
    resp["<string>"] = "   888    888    888  888    888  888         │ │ ╰─╰──────╮";
    responses.push_back(resp);
    resp["<string>"] = "   888    888   d88P  888         888.d888b.  \\ /         ◢▉◣";
    responses.push_back(resp);
    resp["<string>"] = "   888    8888888P\"   888         88Y    Y8h   │          ▉▉▉";
    responses.push_back(resp);
    resp["<string>"] = "   888    888 T88b    888    888  888    888e  │          ◥▉◤";
    responses.push_back(resp);
    resp["<string>"] = "   888    888  T88b   Y88b  d88P  888    888a  │    ╰─┬────╯";
    responses.push_back(resp);
    resp["<string>"] = " 8888888  888   T88b   \"Y8888P\"   888    888r  ╞══════╡";
    responses.push_back(resp);
    resp["<string>"] = "";
    responses.push_back(resp);
    resp["<string>"] = " ──────────────────────── About Us ─────────────────────────";
    responses.push_back(resp);
    resp["<string>"] = "     Meet our IRC mascot, Bentley (also @hulim's pet)!";
    responses.push_back(resp);
    resp["<string>"] = "     Special thanks to @hulim for designing the class";
    responses.push_back(resp);
    resp["<string>"] = "         structure of our IRC server components.";
    responses.push_back(resp);
    resp["<string>"] = "            Created by: @hulim, @anteo, @elfoo";
    responses.push_back(resp);
    resp["<string>"] = "";
    responses.push_back(resp);
    resp["<string>"] = " ────────────────────── Server Rules ───────────────────────";
    responses.push_back(resp);
    resp["<string>"] = "       1. Be kind and respectful to others";
    responses.push_back(resp);
    resp["<string>"] = "       2. No spamming, flooding, or excessive messages";
    responses.push_back(resp);
    resp["<string>"] = "       3. Zero tolerance for hate speech or harassment";
    responses.push_back(resp);
    resp["<string>"] = "       4. Keep conversations family-friendly";
    responses.push_back(resp);
    resp["<string>"] = "";
    responses.push_back(resp);
    resp["<string>"] = " ─────────────────────── Need Help? ────────────────────────";
    responses.push_back(resp);
    resp["<string>"] = "          Check out [https://irssi.org/New-users/]";
    responses.push_back(resp);
    resp["<string>"] = "              Contact an admin for assistance";
    responses.push_back(resp);
    resp["<string>"] = "";
    responses.push_back(resp);
    resp["<string>"] = "                 Thank you for choosing IRCh!";
    responses.push_back(resp);
    resp["<string>"] = "               Enjoy your stay! (ﾉ◕ ヮ◕)ﾉ*:・ﾟ✧";
    responses.push_back(resp);
    resp["<string>"] = "╰───────────────────────────────────────────────────────────╯";
    responses.push_back(resp);

    // RPL_ENDOFMOTD
    resp = createSingleResponse("376", client.getSocketFdString());
    resp["<client>"] = client.getClientPrefix();
    resp["<info>"] = "End of /MOTD command.";
    responses.push_back(resp);

    return responses;
}

std::string CommandBase::intSetToCSVString(std::set<int> fdset)
 {
    std::string result;
    for (std::set<int>::const_iterator it = fdset.begin(); it != fdset.end(); ++it) {
        if (it != fdset.begin()) {
            result += ",";
        }
        std::ostringstream ss;
        ss << *it;
        result += ss.str();
    }
    return result;
}

std::vector<std::string> CommandBase::split(std::string src, std::string delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    if (src.empty())
        return tokens;
    std::size_t pos_start = 0;
    std::size_t pos_last;
    std::size_t delim_len = delimiter.length();
    while ((pos_last = src.find(delimiter, pos_start)) != std::string::npos) {
        token = src.substr(pos_start, pos_last - pos_start);
        tokens.push_back(token);
        pos_start = pos_last + delim_len;
    }
    tokens.push_back(src.substr(pos_start));
    return tokens;
}
