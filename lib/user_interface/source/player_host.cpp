#include "player_host.h"
#include "log.h"
#include "board.h"
#include <mutex>
#include <condition_variable>

namespace Player
{

class PlayerHostImpl : public IHostPlayer
{
public:
    explicit PlayerHostImpl(BoardPlayerType player_type,
                            UserInterfaceHostPlayerCallbacks callbacks):
        IHostPlayer(player_type),
        callbacks_(std::move(callbacks)) {
        LOG_D("PlayerHostImpl created");
        if (!callbacks_.notifyIsHostPlayerTurn || !callbacks_.notifyRoundEnd) {
            throw std::runtime_error("Callbacks not set properly");
        }
    }

    ~PlayerHostImpl() {
        LOG_D("PlayerHostImpl destructor called");
        setPlayerMove(Board::kInvalidMove); // Notify that the player is no longer available
    }

    std::pair<int, int> get_move(const Board::Board &board) override {
        LOG_D("PlayerHostImpl::get_move called");
        if (callbacks_.notifyIsHostPlayerTurn) {
            callbacks_.notifyIsHostPlayerTurn(board);
        } else {
            LOG_E("notifyIsHostPlayerTurn callback is not set");
            throw std::runtime_error("notifyIsHostPlayerTurn callback is not set");
        }

        // Wait for the player to set the move
        LOG_D("Waiting for player move");
        std::unique_lock<std::mutex> lock(player_move_mutex_);
        player_move_cv_.wait(lock, [this] {
            return is_player_move_set_;
        });
        LOG_D("Player move received ({}, {})", player_move_.first, player_move_.second);
        is_player_move_set_ = false; // Reset the flag for the next move
        return player_move_;
    }

    void notifyRoundEnd(RoundResult result, std::pair<int, int> score, size_t round, const Board::BoardType &board) override {
        LOG_D("PlayerHostImpl::notifyRoundEnd called ({}, {})", score.first, score.second);
        if (callbacks_.notifyRoundEnd) {
            callbacks_.notifyRoundEnd(result, score, round, board);
        } else {
            LOG_E("notifyRoundEnd callback is not set");
            throw std::runtime_error("notifyRoundEnd callback is not set");
        }
    }

    void setPlayerMove(std::pair<int, int> move) override {
        LOG_D("PlayerHostImpl::setPlayerMove called ({}, {})", move.first, move.second);
        std::unique_lock<std::mutex> lock(player_move_mutex_);
        player_move_ = move;
        is_player_move_set_ = true;
        lock.unlock(); // Unlock the mutex before notifying
        player_move_cv_.notify_one(); // Notify the waiting thread
    }

private:
    UserInterfaceHostPlayerCallbacks callbacks_;

    std::mutex player_move_mutex_;
    bool is_player_move_set_ = false;
    std::condition_variable player_move_cv_;
    std::pair<int, int> player_move_ {0, 0};
};

PlayerHost::PlayerHost(BoardPlayerType player_type,
                       UserInterfaceHostPlayerCallbacks callbacks):
    IHostPlayer(player_type),
    impl_(std::make_unique<PlayerHostImpl>(player_type, callbacks)) {
}

} // namespace Player
