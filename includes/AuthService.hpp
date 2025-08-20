#ifndef AUTHSERVICE_HPP
#define AUTHSERVICE_HPP

#include <string>

class AuthService {
    public:
        virtual ~AuthService() {};
        virtual bool validatePassword(const std::string& input) const = 0;
};

#endif