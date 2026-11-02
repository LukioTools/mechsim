#include <iostream>
#include <string>
int main() {
    std::string input = "200,200";
    int x = std::stoi(input.substr(0, input.find(',')));
    int y = std::stoi(input);

    std::cout << x << std::endl;
    std::cout << y << std::endl;
}