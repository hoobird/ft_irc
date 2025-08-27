#include "MessageParser.hpp"

MessageParser::MessageParser()
{
}

MessageParser::~MessageParser()
{
}

ParsedMessage MessageParser::parse(const std::string &input)
{
    ParsedMessage parsedMessage;
    std::istringstream iss(input);

    // Check for tags
    if (!input.empty() && input[0] == '@') {
        std::string tagPart;
        iss >> tagPart;
        if (!tagPart.empty()) {
            tagPart.erase(0, 1); // Remove the '@' character
            std::istringstream tagStream(tagPart);
            std::string tag;
            while (std::getline(tagStream, tag, ';')) {
                size_t pos = tag.find('=');
                if (pos != std::string::npos) {
                    std::string key = tag.substr(0, pos);
                    std::string value = tag.substr(pos + 1);
                    parsedMessage.tags[key] = value;
                } else {
                    parsedMessage.tags[tag] = ""; // No value for this tag
                }
            }
        }
    }

     // Skip whitespace after tags
    while (iss.peek() == ' ') iss.get();

    // Check for source (formerly known as prefix)
    if (iss.peek() == ':') {
        std::string sourcePart;
        iss >> sourcePart;
        if (!sourcePart.empty()) {
            parsedMessage.source = sourcePart.substr(1); // Remove the ':' character
        }
    }

    // Skip whitespace after tags
    while (iss.peek() == ' ') iss.get();

    // Read in command
    iss >> parsedMessage.command;

    // Read in parameters
    std::string param;
    while (iss >> param) {
        if (param[0] != ':') {
            parsedMessage.parameters.push_back(param);
        } else {
            // Trailing starts with : so just join the rest into 1 string
            std::string trailingPart = param.substr(1);
            std::string restOfLine;
            std::getline(iss, restOfLine);
            parsedMessage.trailing = trailingPart + restOfLine;
            break;
        }
    }
    return parsedMessage;
}

void MessageParser::printParsedMessage(const ParsedMessage &msg) const
{
    std::cout << "Tags: ";
    for (std::map<std::string, std::string>::const_iterator it = msg.tags.begin(); it != msg.tags.end(); ++it) {
        std::cout << it->first << "=" << it->second << " ";
    }
    std::cout << std::endl;
    std::cout << "Source: '" << msg.source << "'" << std::endl;
    std::cout << "Command: '" << msg.command << "'" << std::endl;
    std::cout << "Parameters: ";
    for (std::vector<std::string>::const_iterator it = msg.parameters.begin(); it != msg.parameters.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    std::cout << "Trailing: " << msg.trailing << std::endl << std::endl;
}
