cmake_minimum_required(VERSION 3.24)
project(SHELL)

set(CMAKE_C_STANDARD 11)
#link_libraries(SDL2)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
add_executable(shell shell.cpp)
