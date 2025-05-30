#pragma once

#include "board.h"

class IBot {
public:
    virtual ~IBot() = default;
    virtual std::pair<int, int> getMove(const Board::BoardType& board,
                                        BoardPlayerType bot_field) = 0;
};
