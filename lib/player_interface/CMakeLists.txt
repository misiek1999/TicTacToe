cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE HEADERS "include/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")


add_library(PlayerLib STATIC ${SOURCES} ${HEADERS})

set(INCLUDE_DIR include)
target_include_directories(PlayerLib PUBLIC ${INCLUDE_DIR})

target_link_libraries(PlayerLib PUBLIC PlayerTypeLib
                                       BoardLib
                                       LogLib)
