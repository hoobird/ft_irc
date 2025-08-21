#ifndef MESSAGEPARSER_HPP
#define MESSAGEPARSER_HPP

#include "types/ParsedMessage.hpp"
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <iostream>

// Incoming message format:
// [@{tags} ] [:{source} ] {command} [{parameters}] [:{trailing}]

class MessageParser {
	public:
		MessageParser();
		~MessageParser();

		ParsedMessage parse(const std::string& input);
		void printParsedMessage(const ParsedMessage& msg) const;

	private:
		// OCF
		MessageParser(const MessageParser& other);
		MessageParser& operator=(const MessageParser& other);
};

#endif
