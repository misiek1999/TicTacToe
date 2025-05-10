#pragma once

#include "board.h"
#include "bot_interface.h"
#include "bot_random.h"
#include "bot_algorithm.h"

#include <memory>

class IBotFactory {
public:
    virtual ~IBotFactory() = default;
    virtual std::unique_ptr<IBot> createBot() = 0;
};

class BotFactoryRandom : public IBotFactory {
public:
    BotFactoryRandom() = default;
    inline virtual std::unique_ptr<IBot> createBot() override {
        return std::make_unique<BotRandom>();
    }
};

class BotFactoryAlgorithm : public IBotFactory {
public:
    BotFactoryAlgorithm() = default;
    inline virtual std::unique_ptr<IBot> createBot() override {
        return std::make_unique<BotAlgorithm>();
    }
};
