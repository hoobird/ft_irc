#ifndef RESPONSETYPE_HPP
#define RESPONSETYPE_HPP

#include <map>
#include <string>
#include <vector>
#include <set>
#include <cstdlib>
#include <iostream>

typedef std::map<std::string, std::string> singleResponse;
typedef std::vector<singleResponse> responseList;

inline void printSingleResponse(const singleResponse& resp) {
    std::cout << "\tSingle Response { " << std::endl;
    for (singleResponse::const_iterator sit = resp.begin(); sit != resp.end(); ++sit) {
        std::cout << "\t\t" << sit->first << " : " << sit->second << std::endl;
    }
    std::cout << "\t}" << std::endl;
}

inline void printReponseList(const responseList& rlist) {
    std::cout << "Response List { " << std::endl;
    for (responseList::const_iterator it = rlist.begin(); it != rlist.end(); ++it) {
        printSingleResponse(*it);
    }
    std::cout << "} " << std::endl;
}

inline std::set<int> parseClientFds(const std::string& fdsStr) {
    std::set<int> fds;
    size_t start = 0;
    size_t end = fdsStr.find(',');
    while (end != std::string::npos) {
        const std::string fdStr = fdsStr.substr(start, end - start);
        char* endptr = NULL;
        int fd = static_cast<int>(std::strtol(fdStr.c_str(), &endptr, 10));
        if (endptr != fdStr.c_str()) {
            fds.insert(fd);
        }
        start = end + 1;
        end = fdsStr.find(',', start);
    }
    // Add the last (or only) fd
    if (start < fdsStr.length()) {
        const std::string fdStr = fdsStr.substr(start);
        char* endptr = NULL;
        int fd = static_cast<int>(std::strtol(fdStr.c_str(), &endptr, 10));
        if (endptr != fdStr.c_str()) {
            fds.insert(fd);
        }
    }
    return fds;
}


// singleResponse example numeric 001 -> <client> :Welcome to the Internet Relay Network <nick>!<user>@<host>

// responseList = {
//     {
//         "<numeric>": "001",
//         "<clientsToSend>": "12,22,33",  // these are client Fds seperated by commas
//          ** ABOVE IS METADATA WE NEED ** 
//          ** BELOW IS REPLACEMENT FOR NUMERICS PLACEHOLDER ** 
//         "<client>": "bob",
//         "<nick>": "bob",
//         "<user>": "bob",
//         "<host>": "localhost"
//     },
//      ...    
// }


#endif
