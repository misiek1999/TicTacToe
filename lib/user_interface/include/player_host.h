#pragma once

#include "player_interface.h"

#include <memory>

namespace Player {

class PlayerHostImpl;

struct UserInterfaceHostPlayerCallbacks {
    std::function<void(const Board::Board &board)> notifyIsHostPlayerTurn;
    std::function<void(RoundResult result, std::pair<int, int> score, size_t round, const Board::BoardType &board)> notifyRoundEnd;
};

class IHostPlayer : public IPlayer {
public:
    IHostPlayer(BoardPlayerType player_type) : IPlayer(player_type) {}
    virtual ~IHostPlayer() = default;
    virtual void setPlayerMove(std::pair<int, int> move) = 0;
};

class PlayerHost : public IHostPlayer {
public:
    PlayerHost(BoardPlayerType player_type,
               UserInterfaceHostPlayerCallbacks callbacks);
    ~PlayerHost() = default;

    std::pair<int, int> get_move(const Board::Board &board) override {
        return impl_->get_move(board);
    }

    void notifyRoundEnd(RoundResult result, std::pair<int, int> score, size_t round, const Board::BoardType &board) override {
        impl_->notifyRoundEnd(result, score, round, board);
    }

    void setPlayerMove(std::pair<int, int> move) override {
        impl_->setPlayerMove(move);
    }

private:
    std::unique_ptr<IHostPlayer> impl_;
};

} // namespace Player
