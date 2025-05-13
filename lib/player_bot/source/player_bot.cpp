#include "player_bot.h"
#include "player_interface.h"
#include "bot_factory.h"

namespace Player
{

class PlayerBotImpl : public IPlayer
{
public:
    explicit PlayerBotImpl(const BoardPlayerType player_type, std::unique_ptr<IBotFactory> factory) :
            IPlayer(player_type){
        // Initialize the bot algorithm
        bot_algorithm_ = factory->createBot();
    }

    std::pair<int, int> get_move(const Board::Board &board) override {
        std::ignore = board;
        const auto player_type = get_player_type();
        const auto move = bot_algorithm_->getMove(board.get_board(), player_type);
        LOG_D("Bot player {} move: row: {}, col: {}", static_cast<int>(player_type), move.first, move.second);
        return  move;
    };

    void notifyRoundEnd(RoundResult result, std::pair<int, int> score, size_t round, const Board::BoardType &board) override {
        std::ignore = result;
        std::ignore = score;
        std::ignore = round;
        std::ignore = board;
    }

private:
    std::unique_ptr<IBot> bot_algorithm_;
};

PlayerBot::PlayerBot(const BoardPlayerType player_type, std::unique_ptr<IBotFactory> factory):
    IPlayer(player_type),
    impl_(std::make_unique<PlayerBotImpl>(player_type, std::move(factory))) {
}

} // namespace Player
