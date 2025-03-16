cmake_minimum_required(VERSION 3.10.0)
project(tictactoe VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

set(INCLUDE_DIR include)
include_directories(${INCLUDE_DIR})

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_subdirectory(lib)

add_executable(tictactoe ${SOURCES})

target_link_libraries(tictactoe PRIVATE logLib)
