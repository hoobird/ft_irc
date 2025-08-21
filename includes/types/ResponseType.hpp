#ifndef RESPONSETYPE_HPP
#define RESPONSETYPE_HPP

#include <map>
#include <string>
#include <vector>

typedef std::map<std::string, std::string> singleReponse;
typedef std::vector<singleReponse> responseList;

// singleReponse example numeric 001 -> <client> :Welcome to the Internet Relay Network <nick>!<user>@<host>

// responseList = {
//     {
//         "<numeric>": "001",
//         "<clients>": "12,22,33",  // these are client Fds seperated by commas
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