#pragma once

#include "player_interface.h"

#include <memory>

namespace Player {

class PlayerHostImpl;

class PlayerHost : public IPlayer {
public:
    explicit PlayerHost(BoardPlayerType player_type);
    ~PlayerHost() = default;

    std::pair<int, int> get_move(const Board::Board &board) override {
        return impl_->get_move(board);
    }

private:
    std::unique_ptr<IPlayer> impl_;
};

} // namespace Player
