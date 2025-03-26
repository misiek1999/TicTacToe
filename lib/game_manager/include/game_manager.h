#pragma once

#include <memory>

#include "log.h"
#include "player_type.h"
#include "board.h"
#include "player_interface.h"

namespace GameManager {

enum class GameType {
    LocalBot,
    RemotePlayer
};

class IGameManager {
public:
    virtual ~IGameManager() = default;
    virtual void startGame() = 0;
    virtual void stopGame() = 0;
    virtual std::pair<int, int> getScore() const = 0;
    virtual size_t getRound() const = 0;

private:
};

class GameManagerImpl;

class GameManager {
public:
    GameManager();
    explicit GameManager(std::shared_ptr<Player::IPlayer> player);
    ~GameManager() = default;
    void startGame() {
        impl_->startGame();
    }

    void stopGame() {
        impl_->stopGame();
    }

    std::pair<int, int> getScore() const {
        return impl_->getScore();
    }

    size_t getRound() const {
        return impl_->getRound();
    }

private:
    std::unique_ptr<IGameManager> impl_;
};

} // namespace GameManager
