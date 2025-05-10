#pragma once

#include "board.h"
#include "bot_interface.h"

class BotAlgorithm : public IBot {
public:
    BotAlgorithm();
    virtual ~BotAlgorithm() = default;
    std::pair<int, int> getMove(const Board::BoardType& board) override;
};
