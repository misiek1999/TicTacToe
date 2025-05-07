#include "game_engine.h"
#include "log.h"


namespace GameEngine {

class GameEngineImpl : public IGameEngine {
public:
    explicit GameEngineImpl(std::shared_ptr<PlayerManager::PlayerManager> playerManagerPtr, size_t board_size):
            playerManagerPtr_(playerManagerPtr),
            board_size_(board_size) {
        LOG_I("Creating game engine with board size: {}", board_size);

    }

    GameEngineError processGame() override {
        auto return_code = GameEngineError::kOK;
        if (is_game_finished_) {
            LOG_W("Game is finished. Please reset the game.");
            return GameEngineError::KBoardNotClear;
        }

        LOG_V("Next game loop");
        auto [current_player, player_type] = this->getCurrentPlayer();

        auto board = Board::Board(board_.get_board());
        const auto [row, col] = current_player->get_move(std::move(board));

        if (!board_.is_valid_move(row, col)) {
            LOG_W("Invalid move");
            return GameEngineError::kInvalidMove;
        }

        auto move_result = board_.make_move(row, col, player_type);
        if (move_result.has_value()) {
            if (board_.is_winner(player_type)) {
                LOG_I("Player {} won", static_cast<int>(player_type));
                if (player_type == BoardPlayerType::X) {
                    guest_player_score_++;
                } else {
                    host_player_score_++;
                }
                is_game_finished_ = true;
                return_code = GameEngineError::kGameFinished;
            } else if (board_.is_full()) {
                LOG_I("Board is full, game finished without winner");
                is_game_finished_ = true;
                return_code = GameEngineError::kGameFinished;
            } else {
                LOG_V("Game continues");
            }
        } else {
            LOG_W("Invalid move, error: {}", static_cast<int>(move_result.error()));
            return GameEngineError::kInvalidMove;
        }

        // if move is valid, change turn
        is_host_turn_ = !is_host_turn_;
        return return_code;
    }

    Board::BoardType getBoard() const override {
        return board_.get_board();
    }

    std::pair<int, int> getScore() const override {
        return {host_player_score_, guest_player_score_};
    }

    void resetGame() override {
        LOG_I("Resetting game engine");
        board_.reset();
        is_host_turn_ = true;
        is_game_finished_ = false;
    }

    void resetBoard() override {
        LOG_I("Resetting board");
        board_.reset();
        is_game_finished_ = false;
    }

private:
    std::shared_ptr<PlayerManager::PlayerManager> playerManagerPtr_;
    [[maybe_unused]] size_t board_size_;
    Board::Board board_;
    size_t host_player_score_{0};
    size_t guest_player_score_{0};
    bool is_host_turn_{true};
    bool is_game_finished_{false};

    std::pair<std::shared_ptr<Player::IPlayer>, BoardPlayerType> getCurrentPlayer() {
        if (is_host_turn_) {
            LOG_V("Host turn");
            return {playerManagerPtr_->getHostClient(), BoardPlayerType::O};
        } else {
            LOG_V("Guest turn");
            return {playerManagerPtr_->getGuestClient(), BoardPlayerType::X};
        }
    }
};

GameEngine::GameEngine(std::shared_ptr<PlayerManager::PlayerManager> playerManagerPtr, size_t board_size) {
    impl_ = std::make_unique<GameEngineImpl>(playerManagerPtr, board_size);
}

}   // namespace GameEngine
