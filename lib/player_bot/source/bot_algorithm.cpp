#include "bot_algorithm.h"

#include "log.h"

BotAlgorithm::BotAlgorithm() {
    LOG_D("BotAlgorithm created\n");
}

std::pair<int, int> BotAlgorithm::getMove(const Board::BoardType& board) {
    std::ignore = board;
    // Generate a random move
    return std::make_pair<int, int>(1, 1);
}
