#include "user_interface.h"
#include "log.h"
#include "game_manager.h"
#include "player_host.h"

#include <atomic>
#include <cstdlib>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#endif // _WIN32

namespace UI
{

class UserInterfaceImpl : public IUserInterface {
public:
    UserInterfaceImpl() {
        player_host_ = std::make_shared<Player::PlayerHost>(BoardPlayerType::X);
        game_manager_ = std::make_unique<GameManager::GameManager>(player_host_);
        LOG_D("User interface created");
    }

    ~UserInterfaceImpl() {
        LOG_D("User interface destroyed");
    }

    void startGame() override {
        LOG_D("Starting game");
        game_manager_->startGame();
        this->processGame();
    }

    void stopGame() override {
        LOG_D("Stopping game");
        is_game_stopped = true;
        game_manager_->stopGame();
    }


private:
    std::shared_ptr<Player::IPlayer> player_host_;
    std::unique_ptr<GameManager::GameManager> game_manager_;

    std::atomic<bool> is_game_stopped = false;

    void clearConsole() {
        #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif
    }

    void pause() {
        std::cout << "Press any key to continue...";
        #ifdef _WIN32
        _getch();
        #else
        std::cin.get();
        #endif
    }

    void printBoard(Board::BoardType board) {
        clearConsole();
        for (const auto &row : board) {
            for (const auto &cell : row) {
                const auto cell_char = Board::convertBoardFieldToChar(cell);
                std::cout << cell_char << " ";
            }
            std::cout << std::endl;
        }
    }

    void gameEndMessage() {
        clearConsole();
        std::cout << "\nDo you want to exit? ESC - Quit\n";

        #ifdef _WIN32
        if (GetAsyncKeyState(VK_ESCAPE)) {
        #else
        // Implement alternative escape detection for other platforms
        #endif
            std::cout << "Goodbye!\n";
            is_game_stopped = true;
        }

        std::cout << "Continuing the game!\n";
    }

    void processGame() {
        while (!is_game_stopped) {
            clearConsole();

            const auto score = game_manager_->getScore();
            const auto round = game_manager_->getRound();
            std::cout << "Host: " << score.first << " Guest: " << score.second << "Round: " << round << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(120));
            game_manager_->stopGame();
        }
    }
};

UserInterface::UserInterface():
    impl_(std::make_unique<UserInterfaceImpl>()) {
}

} // namespace UI
