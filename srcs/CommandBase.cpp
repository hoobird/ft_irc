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
    responses.reserve(5);
    // Numeric 001 to 005
    // TODO: 005 RPL_ISUPPORT might be needed?
    singleResponse resp1 = createSingleResponse("001", client.getSocketFdString());
    resp1["<client>"] = client.getClientPrefix();
    resp1["<nick>"] = client.getNickname();
    resp1["<user>"] = client.getUsername();
    resp1["<host>"] = client.getHostname();
    responses.push_back(resp1);

    singleResponse resp2 = createSingleResponse("002", client.getSocketFdString());
    resp2["<client>"] = client.getClientPrefix();
    resp2["<servername>"] = serverName;
    resp2["<version>"] = "1.0";
    responses.push_back(resp2);

    singleResponse resp3 = createSingleResponse("003", client.getSocketFdString());
    resp3["<client>"] = client.getClientPrefix();
    resp3["<date>"] = "2025-08-23";
    responses.push_back(resp3);

    singleResponse resp4 = createSingleResponse("004", client.getSocketFdString());
    resp4["<client>"] = client.getClientPrefix();
    resp4["<server_name>"] = serverName;
    resp4["<version>"] = "1.0";
    resp4["<usermodes>"] = "";
    resp4["<chanmodes>"] = "";
    responses.push_back(resp4);

    // singleResponse resp5 = createSingleResponse("005", client.getSocketFdString());

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
