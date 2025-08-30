/* taken from: https://stackoverflow.com/questions/46477764/check-stdvector-has-duplicates */
/* compile with: c++ -Wall -Wextra -Werror -std=c++11 hasDuplicate_map.cpp */

// conclusion: we don't need a hasDuplicate()!!

#include <iostream>
#include <vector>
#include <map>

void print(const std::map<std::string, std::string>& map) {
    if (map.empty()) {
        std::cout << "Map is empty" << std::endl;
        return ;
    }
    std::map<std::string, std::string>:: const_iterator it;
    for (it = map.begin(); it != map.end(); ++it) {
        std::cout << "key: " << it->first << ", value: " << it->second << std::endl;
    }
}

int main(void)
{
    // assumption: channelNames and channelKeys have been stored successfully
    std::vector<std::string> channelNames = {"qwaszx_world", "qwerty_world", "abc_world"};
    std::vector<std::string> channelKeys = {"pass", "", "pass"};

    // note: inherent property of std::map to ensure unique keys in key-value pair
    std::map<std::string, std::string> mapChannelKey;
    for (std::string::size_type i = 0; i < channelNames.size(); ++i) {
        if (channelNames.size() <= channelKeys.size())
            mapChannelKey.insert(std::make_pair(channelNames[i], channelKeys[i]));
        else // if channelNames.size() > channelKeys.size()
            mapChannelKey.insert(std::make_pair(channelNames[i], std::string()));
    }
    std::cout << "After storing...\n";
    print(mapChannelKey);

    std::cout << "\nPrinting original...\n";
    std::map<std::string, std::string> mapChannelKey_original;
    mapChannelKey_original.insert({{"qwaszx_world", "pass"}, {"qwerty_world", ""}, {"abc_world", "pass"}});
    print(mapChannelKey_original);
}

/* Output:
c2r2s14% c++ -Wall -Wextra -Werror -std=c++11 hasDuplicate_map.cpp
c2r2s14% ./a.out
After storing...
key: abc_world, value: pass
key: qwerty_world, value: pass

Printing original...
key: abc_world, value: pass
key: qwerty_world, value: pass
c2r2s14% */

// try: change first "qwerty_world" to "qwaszx_world"!

/* Output:
c2r2s14% c++ -Wall -Wextra -Werror -std=c++11 hasDuplicate_map.cpp
c2r2s14% ./a.out
After storing...
key: abc_world, value: pass
key: qwaszx_world, value: pass
key: qwerty_world, value:

Printing original...
key: abc_world, value: pass
key: qwaszx_world, value: pass
key: qwerty_world, value:
c2r2s14% */
