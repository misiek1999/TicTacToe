#pragma once

#include <spdlog/spdlog.h>


// Logging macros
#define LOG_V(...) spdlog::trace(__VA_ARGS__)
#define LOG_D(...) spdlog::debug(__VA_ARGS__)
#define LOG_I(...) spdlog::info(__VA_ARGS__)
#define LOG_W(...) spdlog::warn(__VA_ARGS__)
#define LOG_E(...) spdlog::error(__VA_ARGS__)
#define LOG_F(...) spdlog::critical(__VA_ARGS__)

// Initialize logger
void init_logger();
