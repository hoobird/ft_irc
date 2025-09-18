#include <iostream>
#include <cstdlib> // for strtol
#include <string>

int main() {
    std::string input;
    
    while (true) {
        std::cout << "Enter a number (or 'q' to quit): ";
        std::getline(std::cin, input);
        
        if (input == "q" || input == "Q") {
            break;
        }
        
        char* end;
        long number = strtol(input.c_str(), &end, 10);
        
        std::cout << "\n--- Results ---\n";
        std::cout << "Input string: \"" << input << "\"\n";
        std::cout << "Converted number: " << number << "\n";
        std::cout << "Stopped at character: ";
        
        if (end == input.c_str()) {
            std::cout << "[No conversion performed]";
        } else if (*end == '\0') {
            std::cout << "[End of string]";
        } else {
            std::cout << "'" << *end << "'";
        }
        
        std::cout << "\nRemaining part: \"" << end << "\"\n";
        
        if (end == input.c_str()) {
            std::cout << "ERROR: No valid digits found!\n";
        } else if (*end != '\0') {
            std::cout << "WARNING: Extra characters after number\n";
        } else {
            std::cout << "SUCCESS: Full string converted\n";
        }
        
        std::cout << "----------------\n\n";
    }
    
    return 0;
}