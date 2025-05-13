#include "user_interface.h"
#include "log.h"
#include "game_manager.h"
#include "player_host.h"
#include "console_manager.h"
#include "board.h"

#include <cstdlib>
#include <iostream>
#include <mutex>
#include <semaphore>
#include <queue>

namespace UI
{

enum class UIEventType {
    PlayerMove,
    RoundEnd,
    GameEnd,
    kInvalidEvent
};

constexpr size_t kMaxEventCount = 10U;

class UserInterfaceImpl : public IUserInterface {
public:
    UserInterfaceImpl() {
        Player::UserInterfaceHostPlayerCallbacks callbacks = {
            .notifyIsHostPlayerTurn = std::bind(&UserInterfaceImpl::listenForHostPlayerMove,
                                                this, std::placeholders::_1),
            .notifyRoundEnd = std::bind(&UserInterfaceImpl::listenForRoundEnd,
                                        this, std::placeholders::_1,
                                        std::placeholders::_2,
                                        std::placeholders::_3,
                                        std::placeholders::_4)
        };
        console_manager_ = std::make_unique<ConsoleManager>();
        player_host_ = std::make_shared<Player::PlayerHost>(BoardPlayerType::X,
                                                            std::move(callbacks));
        game_manager_ = std::make_unique<GameManager::GameManager>(player_host_);
        LOG_D("User interface created");
    }

    ~UserInterfaceImpl() {
        LOG_D("User interface destroyed");
        player_host_->setPlayerMove(Board::kInvalidMove); // Notify that the player is no longer available
    }

    void startGame() override {
        LOG_I("Starting game");
        game_manager_->startGame();
        this->processGame();
    }

    void stopGame() override {
        LOG_D("Stopping game");
        addNewEvent(UIEventType::GameEnd);
        game_manager_->stopGame();
    }


private:
    std::shared_ptr<Player::IHostPlayer> player_host_;
    std::unique_ptr<GameManager::GameManager> game_manager_;
    std::unique_ptr<ConsoleManager::IConsoleManager> console_manager_;

    // variables used for event handling
    std::mutex player_move_mutex_;
    std::mutex game_stat_update_mutex_;
    std::mutex event_queue_mutex_;
    std::queue<UIEventType> event_queue_;
    std::counting_semaphore<kMaxEventCount> event_semaphore_{0};

    // copy of the game board
    Board::BoardType game_board_ = {};

    //game stats
    std::pair<int, int> game_score_ = {0, 0};
    size_t game_round_ = 1;
    RoundResult last_round_result_ = RoundResult::Draw;

    void listenForHostPlayerMove(const Board::Board &board) {
        std::ignore = board; // Ignore the board for now
        LOG_D("Host player turn notified");
        {
            std::unique_lock<std::mutex> lock(player_move_mutex_);
            game_board_ = board.get_board(); // Copy the board state
        }
        addNewEvent(UIEventType::PlayerMove); // Notify that the player can make a move
    }

    void listenForRoundEnd(RoundResult result, std::pair<int, int> score,
                           size_t round, const Board::BoardType &board) {
        LOG_D("Round end notified. Result: {}, Score: ({}, {}), Round: {}",
              static_cast<int>(result), score.first, score.second, round);
        {
            std::scoped_lock<std::mutex> lock(game_stat_update_mutex_);
            last_round_result_ = result;
            game_score_ = score;
            game_round_ = round;
        }
        {
            std::scoped_lock<std::mutex> lock(player_move_mutex_);
            game_board_ = board; // Copy the board state
        }
        // Notify the UI to update the game stats
        addNewEvent(UIEventType::RoundEnd);
    }

    void getPlayerMove() {
        try {
            auto player_move = console_manager_->getPlayerMove(getGameBoard());
            player_host_->setPlayerMove(player_move); // Replace with actual move
        } catch (const std::exception &e) {
            LOG_I("Catched exception: {}", e.what());
            stopGame(); // Stop the game on error
        }
    }

    void processGame() {
        auto game_finished = false;
        while (!game_finished) {
            // Clear console before printing the board and score
            console_manager_->clearConsole();
            {
                std::scoped_lock<std::mutex> lock(game_stat_update_mutex_);
                // Print game stats
                console_manager_->printGameStats(game_score_.first,
                                                 game_score_.second,
                                                 game_round_);
            }
            const auto event = getNextEvent();
            switch (event) {
                case UIEventType::PlayerMove: // Get player move
                    getPlayerMove();
                    break;
                case UIEventType::RoundEnd:  // Print game stats
                    console_manager_->printRoundEndMessage(getGameBoard(),
                                                           last_round_result_,
                                                           game_round_ - 1);    // Decrement round for display
                    break;
                case UIEventType::GameEnd:
                    console_manager_->gameEndMessage(); // Print game end message
                    game_finished = true; // Set game finished flag
                    break;
                default:
                    LOG_E("Invalid event type: {}", static_cast<int>(event));
                    break;
            }
        }
        LOG_I("Game stopped, exiting UI loop");
        game_manager_->stopGame();
    }

    void addNewEvent(UIEventType event) {
        std::scoped_lock<std::mutex> lock(event_queue_mutex_);
        if (event_queue_.size() < kMaxEventCount) {
            event_queue_.push(event);
            event_semaphore_.release(); // Notify that a new event is available
        } else {
            LOG_E("Event queue is full, dropping event");
        }
    }

    UIEventType getNextEvent() {
        event_semaphore_.acquire(); // Wait for an event to be available
        std::scoped_lock<std::mutex> lock(event_queue_mutex_);
        if (!event_queue_.empty()) {
            auto event = event_queue_.front();
            event_queue_.pop();
            return event;
        } else {
            LOG_E("Event queue is empty, returning default event");
            return UIEventType::kInvalidEvent; // Return default event if queue is empty
        }
    }

    Board::BoardType getGameBoard() {
        std::scoped_lock<std::mutex> lock(player_move_mutex_);
        return game_board_;
    }
};

UserInterface::UserInterface():
    impl_(std::make_unique<UserInterfaceImpl>()) {
}

} // namespace UI
