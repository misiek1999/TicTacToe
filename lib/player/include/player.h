#pragma once

#include <utility>

#include "log.h"
#include "player_type.h"
#include "board.h"

namespace Player {

    class IPlayer {
    public:
        IPlayer(BoardPlayerType player_type) : player_type_(player_type) {}
        virtual ~IPlayer() = default;
        virtual BoardPlayerType get_player_type() const = 0;
        virtual std::pair<int, int> get_move(const Board::Board &board) = 0;
    private:
        [[maybe_unused]] BoardPlayerType player_type_; // TODO: add usage
    };

}
