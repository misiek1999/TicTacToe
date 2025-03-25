#include "player_bot.h"
#include "player_interface.h"

#include <random>


namespace Player
{

constexpr size_t kMaxGeneratedNumber= 2U;
constexpr size_t kMinGeneratedNumber= 0U;

class PlayerBotImpl : public IPlayer
{
public:
    explicit PlayerBotImpl(BoardPlayerType player_type) :
            IPlayer(player_type),
            gen_(rd_()) {
        distrib = std::uniform_int_distribution<>(kMinGeneratedNumber, kMaxGeneratedNumber);
    }

    std::pair<int, int> get_move(const Board::Board &board) override {
        std::ignore = board;
        return {distrib(gen_), distrib(gen_)};
    };

private:
        // Create a random device and a Mersenne Twister generator seeded with it
        std::random_device rd_;
        std::mt19937 gen_;
        std::uniform_int_distribution<> distrib;
};

PlayerBot::PlayerBot(BoardPlayerType player_type):
    IPlayer(player_type),
    impl_(std::make_unique<PlayerBotImpl>(player_type)) {
}

} // namespace Player
