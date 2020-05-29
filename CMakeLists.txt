cmake_minimum_required(VERSION 3.5)

project(CHIP-8 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(CHIP-8 src/main.cpp)
target_sources(CHIP-8 PRIVATE
    src/cpu.cpp
    src/display.cpp)
target_link_directories(CHIP-8 PRIVATE lib)
target_include_directories(CHIP-8 PUBLIC include)
