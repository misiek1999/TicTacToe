#include <iostream>

#include "log.h"

int main(int, char**){

    // Initialize logger
    init_logger();

    std::cout << "Hello, from tictactoe!\n";

    LOG_V("Hello, from tictactoe!");
    return 0;
}
