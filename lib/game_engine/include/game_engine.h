#pragma once

#include <memory>
#include <utility>
#include "player_manager.h"
#include "board.h"

namespace GameEngine {

enum class GameEngineError {
    kOK,
    kInvalidMove,
    kInvalidPlayer,
    kGameFinished,
    KBoardNotClear
};

class IGameEngine {
public:
    virtual ~IGameEngine() = default;

    virtual GameEngineError processGame() = 0;
    virtual void resetGame() = 0;
    virtual void resetBoard() = 0;

    virtual Board::BoardType getBoard() const = 0;
    virtual std::pair<int, int> getScore() const  = 0;
};

class GameEngineImpl;

class GameEngine : public IGameEngine {

public:
    explicit GameEngine(std::shared_ptr<PlayerManager::PlayerManager> playerManagerPtr, size_t board_size);
    ~GameEngine() = default;

    GameEngineError processGame() override {
        return impl_->processGame();
    }

    Board::BoardType getBoard() const override {
        return impl_->getBoard();
    }

    std::pair<int, int> getScore() const override {
        return impl_->getScore();
    }

    void resetGame() override {
        impl_->resetGame();
    }

    void resetBoard() override {
        impl_->resetBoard();
    }

private:
    std::unique_ptr<IGameEngine> impl_;
};

} // namespace GameManager
