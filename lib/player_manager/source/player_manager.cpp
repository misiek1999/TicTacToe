#include "player_manager.h"
#include "log.h"
#include "player_bot.h"
#include <unordered_map>
#include <exception>

namespace PlayerManager
{

class PlayerManagerImpl : public IPlayerManager {
public:
    PlayerManagerImpl(TypeOfGuestPlayer type, std::shared_ptr<Player::IPlayer> host):
            type_(type) {
        LOG_D("Selected type of guest player: {}", static_cast<int>(type));
        if (host == nullptr) {
            LOG_E("Host player is nullptr");
            throw std::runtime_error("Host player is nullptr");
        }
        host_client_ = host;
        LOG_V("Host player created");
        createGuestPlayer(type);
    }

    explicit PlayerManagerImpl(TypeOfGuestPlayer type):
            type_(type) {
        LOG_D("Selected type of guest player: {}", static_cast<int>(type));
        host_client_ = std::make_shared<Player::PlayerBot>(BoardPlayerType::X);
        LOG_V("Host player created");
        createGuestPlayer(type);
    }

    std::shared_ptr<Player::IPlayer> getHostClient() override {
        return host_client_;
    }

    std::shared_ptr<Player::IPlayer> getGuestClient() override {
        return guest_client_;
    }

    TypeOfGuestPlayer getGuestPlayerType() override {
        return type_;
    }

private:
    TypeOfGuestPlayer type_;

    std::shared_ptr<Player::IPlayer> host_client_;
    std::shared_ptr<Player::IPlayer> guest_client_;

    void createGuestPlayer(TypeOfGuestPlayer type) {
        std::ignore = type;
        // TODO: Implement player creation based on type
        guest_client_ = std::make_shared<Player::PlayerBot>(BoardPlayerType::O);
        LOG_V("Guest player created, type: {}", static_cast<int>(type_));
    }
};


PlayerManager::PlayerManager(TypeOfGuestPlayer type):
    impl_(std::make_unique<PlayerManagerImpl>(type)) {
}

PlayerManager::PlayerManager(TypeOfGuestPlayer type, std::shared_ptr<Player::IPlayer> host):
    impl_(std::make_unique<PlayerManagerImpl>(type, host)) {
}
} // namespace Player
