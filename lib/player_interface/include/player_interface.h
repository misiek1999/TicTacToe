#pragma once

#include <utility>

#include "log.h"
#include "game_result_type.h"
#include "player_type.h"
#include "board.h"

namespace Player {

using PlayerReadyCallback = std::function<void()>;
using PlayerReadyCallbackId = size_t;

class IPlayer {
public:
    IPlayer(BoardPlayerType player_type) : player_type_(player_type) {}
    virtual ~IPlayer() = default;
    virtual std::pair<int, int> get_move(const Board::Board &board) = 0;
    BoardPlayerType get_player_type() { return player_type_; }
private:
    BoardPlayerType player_type_;
};

}
