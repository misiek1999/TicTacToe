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

private:
    std::unique_ptr<IGameManager> impl_;
};

} // namespace GameManager
