cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE HEADERS "include/*.h")


add_library(PlayerLib STATIC ${HEADERS})

set(INCLUDE_DIR include)
target_include_directories(PlayerLib PUBLIC ${INCLUDE_DIR})

target_link_libraries(PlayerLib PUBLIC PlayerTypeLib
                                       BoardLib
                                       GameTypesLib
                                       LogLib)
