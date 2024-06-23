#include <iostream>
#include <string>

int main() {
    std::string input;
    std::cout << "Enter your input (including spaces): ";
    std::getline(std::cin, input); // Read the entire line, including spaces
    std::cout << "You entered: " << input << std::endl;
    return 0;
}