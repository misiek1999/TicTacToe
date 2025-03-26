#include "player_host.h"

namespace Player
{

class PlayerHostImpl : public IPlayer
{
public:
    explicit PlayerHostImpl(BoardPlayerType player_type):
        IPlayer(player_type) {
    }

    std::pair<int, int> get_move(const Board::Board &board) override {
        std::ignore = board;
        return {0, 0};
    }

private:
};

PlayerHost::PlayerHost(BoardPlayerType player_type):
    IPlayer(player_type),
    impl_(std::make_unique<PlayerHostImpl>(player_type)) {
}

} // namespace Player
