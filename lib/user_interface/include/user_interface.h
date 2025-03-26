#pragma once

#include "player_interface.h"

namespace UI {

class IUserInterface {
public:
    virtual ~IUserInterface() = default;
    virtual void startGame() = 0;
    virtual void stopGame() = 0;
};

class UserInterfaceImpl;

class UserInterface : public IUserInterface {
public:
    UserInterface();
    ~UserInterface() = default;
    void startGame() override {
        impl_->startGame();
    }

    void stopGame() override {
        impl_->stopGame();
    }
private:
    std::unique_ptr<IUserInterface> impl_;
};

} // namespace GameManager
