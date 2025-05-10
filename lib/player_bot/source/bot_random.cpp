#include "bot_random.h"

constexpr size_t kMaxGeneratedNumber= 2U;
constexpr size_t kMinGeneratedNumber= 0U;

BotRandom::BotRandom():
        gen_(rd_()) {
    distrib = std::uniform_int_distribution<>(kMinGeneratedNumber, kMaxGeneratedNumber);
}

std::pair<int, int> BotRandom::getMove(const Board::BoardType& board) {
    std::ignore = board;
    return std::make_pair<int,int>(distrib(gen_), distrib(gen_));
}
