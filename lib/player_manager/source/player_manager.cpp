#include "player_manager.h"
#include "log.h"
#include "player_bot.h"
#include <unordered_map>

namespace PlayerManager
{

class PlayerManagerImpl : public IPlayerManager {
public:
    explicit PlayerManagerImpl(TypeOfGuestPlayer type):
            type_(type) {
        LOG_D("Selected type of guest player: {}", static_cast<int>(type));
        // TODO: Implement player creation based on type
        host_client_ = std::make_shared<Player::PlayerBot>(BoardPlayerType::X);
        LOG_V("Host player created");
        guest_client_ = std::make_shared<Player::PlayerBot>(BoardPlayerType::O);
        LOG_V("Guest player created");
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

};


PlayerManager::PlayerManager(TypeOfGuestPlayer type):
    impl_(std::make_unique<PlayerManagerImpl>(type)) {
}

} // namespace Player
