#include "bot_algorithm.h"

#include "log.h"
#include "board.h"

#include <utility>
#include <ranges>

using Move = std::pair<int, int>;

class TicTacToeAlgorithm : public ITicTacToeAlgorithm {
public:
    TicTacToeAlgorithm() = default;
    ~TicTacToeAlgorithm() = default;

    Move getMove(const Board::BoardType& board, BoardPlayerType bot_field) override {
        bot_field_ = bot_field;
        player_field_ = (bot_field == BoardPlayerType::X) ? BoardPlayerType::O : BoardPlayerType::X;
        // Check is it possible to win
        if (auto winning_move = checkIsWinningMove(board, bot_field_)) {
            LOG_D("Bot winning move found at ({}, {})", winning_move->first, winning_move->second);
            return *winning_move;
        }
        // Check is it possible to block the opponent
        if (auto blocking_move = checkIsWinningMove(board, player_field_)) {
            LOG_D("Bot blocking move found at ({}, {})", blocking_move->first, blocking_move->second);
            return *blocking_move;
        }
        // Check is it possible to make a random move
        const auto& best_move = getBestMove(board);
        LOG_D("Bot move at ({}, {})", best_move.first, best_move.second);
        // Implement the algorithm to find the best move
        return best_move;
    }

private:
    constexpr static int kWinScore = 10;
    constexpr static int kLoseScore = -10;
    constexpr static int kDrawScore = 0;
    constexpr static int kCenterBonus = 1;
    constexpr static int kCornerBonus = 2;

    BoardPlayerType player_field_ = BoardPlayerType::X;
    BoardPlayerType bot_field_ = BoardPlayerType::O;

    std::optional<Move> checkIsWinningMove(const Board::BoardType& board, BoardPlayerType player) {
        // Check all possible moves
        for (const auto [x_ind, x_val] : std::views::enumerate(board)) {
            for (const auto [y_ind, y_val] : std::views::enumerate(x_val)) {
                // Check if the field is empty
                if (y_val == Board::BoardField::EMPTY) {
                    // Create a copy of the board
                    Board::Board board_copy {board};
                    // Make a move
                    if(board_copy.make_move(x_ind, y_ind, player).value_or(false)) {
                        const auto col = static_cast<int>(x_ind);
                        const auto row = static_cast<int>(y_ind);
                        LOG_D("Bot winning move found at ({}, {})", col, row);
                        // Return the winning move
                        return std::make_pair(col, row);
                    }
                }
            }
        }
        // When player is not able to win
        return std::nullopt;
    }

    int getLastMoveScore(const Board::BoardType& board, size_t depth) {
        Board::Board board_copy {board};
        // Check if the game is over
        if (board_copy.is_winner(bot_field_)) {
            return kWinScore - depth;
        } else if (board_copy.is_winner(player_field_)) {
            return kLoseScore + depth;
        } else if (board_copy.is_full()) {
            return kDrawScore;
        }
        return std::numeric_limits<int>::max();
    }

    int minMax(Board::BoardType board, size_t depth = 0) {
        // Check if the game is over
        auto score = getLastMoveScore(board, depth);
        if (score != std::numeric_limits<int>::max()) {
            return score;
        }
        // Check who turn will be in this move
        BoardPlayerType current_player = (depth % 2 == 0) ? bot_field_ : player_field_;
        // Check all possible moves
        int best_score = 0;
        if (depth % 2 == 0) {
            best_score = std::numeric_limits<int>::min();
        } else {
            best_score = std::numeric_limits<int>::max();
        }
        for (const auto [x_ind, x_val] : std::views::enumerate(board)) {
            for (const auto [y_ind, y_val] : std::views::enumerate(x_val)) {
                // Check if the field is empty
                if (y_val == Board::BoardField::EMPTY) {
                    // Create a copy of the board
                    Board::Board board_copy {board};
                    // Make a move
                    const auto res = board_copy.make_move(x_ind, y_ind, current_player);
                    if(res.has_value()) {
                        // Get the score of the move
                        int score_minmax = minMax(board_copy.get_board(), depth + 1);
                        if (depth % 2 == 0) {
                            best_score = std::max(best_score, score_minmax);
                        } else {
                            best_score = std::min(best_score, score_minmax);
                        }
                    } else {
                        LOG_V("Error making bot move: {}", static_cast<int>(res.error()));
                    }
                }
            }
        }
        // Implement the minimax algorithm
        return best_score;
    }

    Move getBestMove(const Board::BoardType& board) {
        // Check all possible moves
        int max_score = std::numeric_limits<int>::min();
        Move best_move = Board::kInvalidMove;
        for (const auto [x_ind, x_val] : std::views::enumerate(board)) {
            for (const auto [y_ind, y_val] : std::views::enumerate(x_val)) {
                // Check if the field is empty
                if (y_val == Board::BoardField::EMPTY) {
                    // Create a copy of the board
                    Board::Board board_copy {board};
                    // Make a move
                    const auto res = board_copy.make_move(x_ind, y_ind, bot_field_);
                    if (!res.has_value()) {
                        // Get the score of the move
                        LOG_V("Error making bot move: {}", static_cast<int>(res.error()));
                        throw std::runtime_error("Error making bot move");
                    }
                    int score = minMax(board_copy.get_board());
                    if (score > max_score) {
                        max_score = score;
                        best_move = std::make_pair(static_cast<int>(x_ind), static_cast<int>(y_ind));
                    }
                }
            }
        }
        LOG_D("Best move found at ({}, {}) with score {}", best_move.first, best_move.second, max_score);
        return best_move;
    }
};

BotAlgorithm::BotAlgorithm() {
    LOG_D("BotAlgorithm created\n");
    algorithm_ = std::make_unique<TicTacToeAlgorithm>();
}

Move BotAlgorithm::getMove(const Board::BoardType& board,
                                          BoardPlayerType bot_field) {
    std::ignore = bot_field;
    // Generate a random move
    const auto& move = algorithm_->getMove(board, bot_field);
    LOG_D("BotAlgorithm::getMove: move = ({}, {})\n", move.first, move.second);
    return move;
}
