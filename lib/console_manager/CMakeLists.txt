cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE HEADERS "include/*.h")
file(GLOB_RECURSE SOURCES "source/*.cpp")


add_library(ConsoleManagerLib STATIC ${SOURCES} ${HEADERS})

set(INCLUDE_DIR include)
target_include_directories(ConsoleManagerLib PUBLIC ${INCLUDE_DIR})

target_link_libraries(ConsoleManagerLib PUBLIC BoardLib
                                               GameTypesLib
                                               LogLib)
