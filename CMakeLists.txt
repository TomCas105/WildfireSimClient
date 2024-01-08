cmake_minimum_required(VERSION 3.18)
project(WildfireClient)

set(CMAKE_CXX_STANDARD 17)

add_executable(WildfireClient main.cpp
        Tiles.cpp
        Tiles.h
        color.hpp
        TileMap.cpp
        TileMap.h
        ClientSocket.cpp
        ClientSocket.h
)

target_link_libraries(WildfireClient wsock32 ws2_32)
