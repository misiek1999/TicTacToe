#include <iostream>

#include "game_manager.h"
#include "log.h"

int main(int, char**){

    // Initialize logger
    init_logger();

    GameManager::GameManager game_manager;
    std::cout << "Hello, from tictactoe!\n";

    game_manager.startGame();

    std::this_thread::sleep_for(std::chrono::seconds(120));

    
    game_manager.stopGame();


    LOG_V("Hello, from tictactoe!");
    return 0;
}
