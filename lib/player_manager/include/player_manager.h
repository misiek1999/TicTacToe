#pragma once

#include <utility>

#include "player_interface.h"

namespace PlayerManager {

enum class TypeOfGuestPlayer {
    Bot,
    Remote
};

class IPlayerManager {
public:
    virtual ~IPlayerManager() = default;
    // Get host and guest clients instances
    virtual std::shared_ptr<Player::IPlayer> getHostClient() = 0;
    virtual std::shared_ptr<Player::IPlayer> getGuestClient() = 0;
    // get player type
    virtual TypeOfGuestPlayer getGuestPlayerType() = 0;

    virtual void notifyPlayersRoundEnd(RoundResult result,
                                      std::pair<int, int> score,
                                      size_t round,
                                      const Board::BoardType &board) = 0;
};

class PlayerManagerImpl;

class PlayerManager : public IPlayerManager {
public:
    PlayerManager(TypeOfGuestPlayer type, std::shared_ptr<Player::IPlayer> host);
    explicit PlayerManager(TypeOfGuestPlayer type);
    ~PlayerManager() = default;
    // Get host and guest clients instances
    std::shared_ptr<Player::IPlayer> getHostClient() override {
        return impl_->getHostClient();
    }

    std::shared_ptr<Player::IPlayer> getGuestClient() override {
        return impl_->getGuestClient();
    }

    TypeOfGuestPlayer getGuestPlayerType() override {
        return impl_->getGuestPlayerType();
    }

    void notifyPlayersRoundEnd(RoundResult result,
                              std::pair<int, int> score,
                              size_t round,
                              const Board::BoardType &board) override {
        impl_->notifyPlayersRoundEnd(result, score, round, board);
    }
private:
    std::unique_ptr<IPlayerManager> impl_;
};

}   // namespace PlayerManager
