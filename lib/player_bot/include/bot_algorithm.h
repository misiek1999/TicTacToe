#pragma once

#include "board.h"
#include "bot_interface.h"

class ITicTacToeAlgorithm {
public:
    virtual ~ITicTacToeAlgorithm() = default;
    virtual std::pair<int, int> getMove(const Board::BoardType& board,
                                        BoardPlayerType bot_field) = 0;
};

class TicTacToeAlgorithm;

class BotAlgorithm : public IBot {
public:
    BotAlgorithm();
    virtual ~BotAlgorithm() = default;
    std::pair<int, int> getMove(const Board::BoardType& board,
                                BoardPlayerType bot_field) override;

private:
    std::unique_ptr<ITicTacToeAlgorithm> algorithm_;
};
