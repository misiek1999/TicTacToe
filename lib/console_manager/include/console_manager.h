#pragma once

#include <memory>
#include <utility>

#include "board.h"
#include "game_result_type.h"

//TODO: Change this to cmake level include system
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#endif // _WIN32

class IConsoleManager {
public:
    virtual ~IConsoleManager() = default;
    virtual void printBoard(const Board::BoardType& board) = 0;
    virtual void printGameStats(int host_score, int guest_score, size_t round) = 0;
    virtual void printRoundEndMessage(const Board::BoardType& board, RoundResult result, size_t round) = 0;
    virtual void clearConsole() = 0;
    virtual void pauseConsole() = 0;
    virtual void gameEndMessage() = 0;
    virtual std::pair<int, int> getPlayerMove(const Board::BoardType& board) = 0;
};

class ConsoleManager : public IConsoleManager {
public:
    ConsoleManager();
    ~ConsoleManager() override = default;

    void printBoard(const Board::BoardType& board) override {
        impl_->printBoard(board);
    }

    void printGameStats(int host_score, int guest_score, size_t round) override {
        impl_->printGameStats(host_score, guest_score, round);
    }

    void printRoundEndMessage(const Board::BoardType& board, RoundResult result, size_t round) override {
        impl_->printRoundEndMessage(board, result, round);
    }

    void clearConsole() override {
        impl_->clearConsole();
    }

    void pauseConsole() override {
        impl_->pauseConsole();
    }

    void gameEndMessage() override {
        impl_->gameEndMessage();
    }

    std::pair<int, int> getPlayerMove(const Board::BoardType& board) override {
        return impl_->getPlayerMove(board);
    }

private:
    std::unique_ptr<IConsoleManager> impl_;
};
