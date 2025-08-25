#ifndef MESSAGEBUILDER_HPP
#define MESSAGEBUILDER_HPP

#include "types/ResponseType.hpp"
#include <iostream>
#include <string>
#include <map>
#include <set>

class MessageBuilder {
	public:
		MessageBuilder(const std::string &serverName);
		~MessageBuilder();
		
		std::string buildFromTemplate(const singleResponse &replacements);
		
	private:
		const std::string &serverName;
		std::map<std::string, std::string> numericTemplates;
		void initializeNumericTemplates();
		
		// OCF
		MessageBuilder();
		MessageBuilder(const MessageBuilder& other);
		MessageBuilder& operator=(const MessageBuilder& other);
};

#endif
