#ifndef PARSEDMESSAGE_HPP
#define PARSEDMESSAGE_HPP

#include <vector>
#include <map>
#include <string>

struct ParsedMessage {
    std::map<std::string, std::string> tags;
    std::string source;
    std::string command;
    std::vector<std::string> parameters;
    std::string trailing;
    bool trailingExists;
};

#endif