#include <iostream>

#include "game_manager.h"
#include "user_interface.h"
#include "log.h"

int main(int, char**){
    std::cout << "Hello, from tictactoe!\n";
    // Initialize logger
    init_logger();

    UI::UserInterface user_interface;
    user_interface.startGame();

    std::this_thread::sleep_for(std::chrono::seconds(5));

    user_interface.stopGame();

    LOG_V("Hello, from tictactoe!");
    return 0;
}
