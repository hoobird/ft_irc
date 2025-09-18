/* compile with: c++ -Wall -Wextra -Werror -std=c++11 find_str_vector_in_set.cpp */

#include <vector>
#include <string>
#include <set>
#include <iostream>

int main(void) {
    std::vector<std::string> myVector = {"apple", "banana", "orange", "grape"};
    std::set<std::string> mySet = {"banana", "kiwi", "grapefruit"};

    for (std::vector<std::string>::const_iterator it = myVector.begin(); it != myVector.end(); ++it) {
        if (mySet.find(*it) != mySet.end()) { // find() returns iterator to element if found, end() otherwise
            std::cout << "String \"" << *it << "\" from vector is present in the set." << std::endl;
        } else {
            std::cout << "String \"" << *it << "\" from vector is NOT present in the set." << std::endl;
        }
    }
    return 0;
}

/* Output
c2r5s14% c++ -Wall -Wextra -Werror -std=c++11 find_str_vector_in_set.cpp
c2r5s14% ./a.out
String "apple" from vector is NOT present in the set.
String "banana" from vector is present in the set.
String "orange" from vector is NOT present in the set.
String "grape" from vector is NOT present in the set.
c2r5s14% */

// ABOVE checks if string in std::vector is present in std::set
// BELOW checks if string in std::vector is present in std::vector

/* for (std::vector<std::string>::const_iterator it = myVector1.begin();
        it != myVector1.end(); ++it) {
    if (std::find(myVector2.begin(), myVector2.end(), *it) != myVector2.end()) {
        std::cout << "String \"" << *it << "\" from vector1 is present in vector2." << std::endl;
    } else {
        std::cout << "String \"" << *it << "\" from vector1 is NOT present in vector2." << std::endl;
    }
} */
