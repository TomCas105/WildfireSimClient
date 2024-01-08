cmake_minimum_required(VERSION 3.26)
project(WildfireSimulator)

set(CMAKE_CXX_STANDARD 17)

add_executable(WildfireSimulator main.cpp
        Tiles.cpp
        Tiles.h
        color.hpp
        TileMap.cpp
        TileMap.h
        ClientSocket.cpp)

target_link_libraries(WildfireSimulator wsock32 ws2_32)
