/* taken from: https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c */
/* compile with: c++ -Wall -Wextra -Werror -std=c++98 split.cpp */

#include <iostream>
#include <vector>
#include <string>

std::vector<std::string> split(std::string src, std::string delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    // method (1): input string (src) is modified
    /* std::size_t pos = 0;
    while ((pos = src.find(delimiter)) != std::string::npos) {
        token = src.substr(0, pos);
        tokens.push_back(token);
        src.erase(0, pos + delimiter.length());
    } */

    // method (2): input string (src) is not modified
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

int main(void) {
    std::string src = "scott>=tiger>=mushroom";
    std::cout << "src (before): " << src << std::endl;
    std::string delimiter = ">=";
    std::vector<std::string> dest = split(src, delimiter); // ["scott", "tiger", "mushroom"]
    std::cout << "printing dest: " << std::endl;
    for (std::size_t i = 0; i < dest.size(); ++i) {
        std::cout << "dest[" << i << "]: " << dest[i] << std::endl;
    }
    std::cout << "src (after): " << src << std::endl;
    return 0;
}

/* Output: // method(1)
c2r5s14% c++ -Wall -Wextra -Werror -std=c++98 split.cpp
c2r5s14% ./a.out
src (before): scott>=tiger>=mushroom
printing dest:
dest[0]: scott
dest[1]: tiger
dest[2]: mushroom
src (after): mushroom
c2r5s14% */

/* Output: // method(2)
c2r5s14% c++ -Wall -Wextra -Werror -std=c++98 split.cpp
c2r5s14% ./a.out
src (before): scott>=tiger>=mushroom
printing dest:
dest[0]: scott
dest[1]: tiger
dest[2]: mushroom
src (after): scott>=tiger>=mushroom
c2r5s14% */
