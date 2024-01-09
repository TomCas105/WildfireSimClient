cmake_minimum_required(VERSION 3.18)
project(WildfireSimClient)

set(CMAKE_CXX_STANDARD 17)

add_executable(WildfireSimClient main.cpp
        Tiles.cpp
        Tiles.h
        color.hpp
        ClientSocket.cpp
        ClientSocket.h
)

target_link_libraries(WildfireSimClient wsock32 ws2_32)
