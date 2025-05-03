#include "game_manager.h"

#include "game_engine.h"
#include "game_result_type.h"
#include "player_manager.h"
#include "log.h"

#include <cstdlib>
#include <atomic>
#include <thread>
#include <memory>
#include <mutex>

namespace GameManager
{

class GameManagerImpl : public IGameManager {
private:
    mutable std::mutex game_thread_mutex_;

public:
    explicit GameManagerImpl(std::shared_ptr<Player::IPlayer> host_player = nullptr) {
        createPlayerManager(host_player);
        createGameEngine();
        LOG_D("Game manager created");
    }

    ~GameManagerImpl() {
        LOG_D("Wait for game thread to stop");
        if (game_thread_.joinable()) {
            game_thread_stopped_ = true;
            game_thread_.join();
        }
        LOG_D("Game manager destroyed");
    }

    void startGame() override {
        LOG_D("Game Manager starting game");
        game_thread_ = std::thread(&GameManagerImpl::gameThraedLoop, this);
    }

    void stopGame() override {
        LOG_D("Game Manager stopping game");
        game_thread_stopped_ = true;
    }

    std::pair<int, int> getScore() const override {
        std::unique_lock<std::mutex> lock(game_thread_mutex_);
        return game_engine_->getScore();
    }

    size_t getRound() const override {
        return round_counter_;
    }

private:
    std::shared_ptr<PlayerManager::PlayerManager> player_manager_;
    std::unique_ptr<GameEngine::GameEngine> game_engine_;

    size_t round_counter_ = 0;

    std::thread game_thread_;
    std::atomic<bool> game_thread_stopped_ = false;

    void createPlayerManager(std::shared_ptr<Player::IPlayer> host_player) {
        if (host_player != nullptr) {
            player_manager_ = std::make_shared<PlayerManager::PlayerManager>(PlayerManager::TypeOfGuestPlayer::Bot, host_player);
        } else {
            LOG_D("Creating player manager with bot player");
            player_manager_ = std::make_shared<PlayerManager::PlayerManager>(PlayerManager::TypeOfGuestPlayer::Bot);
        }
        if (player_manager_ == nullptr) {
            LOG_E("Player manager creation failed");
            throw std::runtime_error("Player manager creation failed");
        }
        if (player_manager_->getGuestClient() == nullptr) {
            LOG_E("Guest player creation failed");
            throw std::runtime_error("Guest player creation failed");
        }
        if (player_manager_->getHostClient() == nullptr) {
            LOG_E("Host player creation failed");
            throw std::runtime_error("Host player creation failed");
        }
        LOG_D("Player manager created successfully");
    }

    void createGameEngine() {
        game_engine_ = std::make_unique<GameEngine::GameEngine>(player_manager_, 3);
        LOG_D("Game engine created");
    }

    void gameThraedLoop() {
        while (true) {
            if (game_thread_stopped_) {
                LOG_D("Game thread stopped");
                break;
            }
            std::unique_lock<std::mutex> lock(game_thread_mutex_);
            const auto game_process_resolutes = game_engine_->processGame();
            LOG_D("Game result: {}", static_cast<int>(game_process_resolutes));
            if (game_process_resolutes == GameEngine::GameEngineError::kGameFinished) {
                LOG_D("Game finished. Resoluts: Host: {}, Guest: {}", game_engine_->getScore().first, game_engine_->getScore().second);
                game_engine_->resetGame();
                ++round_counter_;
            }
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
};

GameManager::GameManager() : impl_(std::make_unique<GameManagerImpl>()) {}

GameManager::GameManager(std::shared_ptr<Player::IPlayer> player) : impl_(std::make_unique<GameManagerImpl>(player)) {}

} // namespace GameManager
