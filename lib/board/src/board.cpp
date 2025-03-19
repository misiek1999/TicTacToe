#include "board.h"
#include <ranges>
#include <algorithm>

namespace Board {

class BoardImpl : public IBoard{
public:
    BoardImpl() {
        this->reset();
    }

    ~BoardImpl() = default;

    BoardType get_board() const override{
        return board_;
    }

    bool is_full() const override {
        return not std::ranges::any_of(board_, [](const auto& row) {
            return std::ranges::any_of(row, [](const auto& field) {
                return field == BoardField::EMPTY;
            });
        });
    }

    bool is_winner(BoardPlayerType player) const override {
        const auto board_player = convertPlayerTypeToBoardField(player);
        for (size_t i = 0; i < kBoardSize; ++i) {
            if (board_[i][0] == board_player && board_[i][1] == board_player && board_[i][2] == board_player) {
                return true;
            }
            if (board_[0][i] == board_player && board_[1][i] == board_player && board_[2][i] == board_player) {
                return true;
            }
        }
        if (board_[0][0] == board_player && board_[1][1] == board_player && board_[2][2] == board_player) {
            return true;
        }
        if (board_[0][2] == board_player && board_[1][1] == board_player && board_[2][0] == board_player) {
            return true;
        }
        return false;
    }

    bool is_valid_move(int row, int col) const override {
        if (row < 0 || row >= static_cast<int>(kBoardSize) || col < 0 || col >= static_cast<int>(kBoardSize)) {
            return false;
        }
        return board_[row][col] == BoardField::EMPTY;
    }

    std::expected<bool, BoardError> make_move(int row, int col, BoardPlayerType player) override {
        if (!is_valid_move(row, col)) {
            return std::unexpected(BoardError::INVALID_MOVE);
        }
        const auto board_player = convertPlayerTypeToBoardField(player);
        if (board_player != BoardField::EMPTY) {
            return std::unexpected(BoardError::INVALID_PLAYER);
        }
        board_[row][col] = board_player;
        return true;
    }

    void reset() override {
        auto flat_view = board_ | std::views::join;
        std::ranges::fill(flat_view, BoardField::EMPTY);
    }
private:
    BoardType board_;
};

// This constructor needs to be defined in the .cpp file after Implementation of the BoardImpl class, due to the unique_ptr initizalization
Board::Board() : board_impl_(std::make_unique<BoardImpl>()) {
}

} // namespace Board
