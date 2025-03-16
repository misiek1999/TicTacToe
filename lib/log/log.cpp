#include "log.h"

void init_logger() {
    spdlog::set_level(spdlog::level::trace);
    spdlog::set_pattern("[%H:%M:%S.%e][%L][tid: %t] %v");
    spdlog::info("Logger initialized");
}
