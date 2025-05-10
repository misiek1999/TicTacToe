#pragma once

#include "board.h"
#include "bot_interface.h"

#include <random>
#include <utility>

class BotRandom : public IBot {
    public:
        BotRandom();
        virtual ~BotRandom() = default;
        std::pair<int, int> getMove(const Board::BoardType& board) override;
    private:
        // Create a random device and a Mersenne Twister generator seeded with it
        std::random_device rd_;
        std::mt19937 gen_;
        std::uniform_int_distribution<> distrib;
};
