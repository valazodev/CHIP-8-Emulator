cmake_minimum_required(VERSION 3.5)

project(CHIP-8 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

add_executable(CHIP-8 src/main.cpp)

target_include_directories(CHIP-8 PUBLIC include)

target_link_directories(CHIP-8 PRIVATE lib/)

target_link_libraries(CHIP-8
    -lmingw32
    -lSDL2main
    -lSDL2
)

target_sources(CHIP-8 PRIVATE
    src/cpu.cpp
    #src/disassembler.cpp
    src/io.cpp
    src/timer.cpp
)
