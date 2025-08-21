#ifndef MESSAGEBUILDER_HPP
#define MESSAGEBUILDER_HPP

#include "ResponseType.hpp"
#include <iostream>
#include <string>
#include <map>
#include <set>

class MessageBuilder {
	public:
		MessageBuilder();
		~MessageBuilder();

		std::string buildFromTemplate(std::string numeric, const singleReponse &replacements);

	private:
		std::map<std::string, std::string> numericTemplates;
		void initializeNumericTemplates();

		// OCF
		MessageBuilder(const MessageBuilder& other);
		MessageBuilder& operator=(const MessageBuilder& other);
};

#endif
