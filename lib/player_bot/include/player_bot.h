#pragma once

#include "player_interface.h"

#include "bot_factory.h"

#include <utility>

namespace Player {

class PlayerBotImpl;

class PlayerBot : public IPlayer {
public:
    explicit PlayerBot(const BoardPlayerType player_type, std::unique_ptr<IBotFactory> factory);
    ~PlayerBot() = default;

    std::pair<int, int> get_move(const Board::Board &board) override {
        return impl_->get_move(board);
    }

    void notifyRoundEnd(RoundResult result, std::pair<int, int> score, size_t round, const Board::BoardType &board) override {
        impl_->notifyRoundEnd(result, score, round, board);
    }

private:
    std::unique_ptr<IPlayer> impl_;
};

}
