//
// Created by tomas on 8. 1. 2024.
//
#include <iostream>
#include <fstream>
#include <sstream>
#include "Tiles.h"
#include <windows.h>
#include "ClientSocket.h"

#ifndef WILDFIRESIMULATOR_TILEMAP_H
#define WILDFIRESIMULATOR_TILEMAP_H

class TileMap {
private:
    std::vector<std::vector<Tile>> _map;
    std::vector<std::vector<Tile>> _mapBuffer;
    int _mapSize;
    int _windDirection;
    int _windDuration;
    std::vector<TileType> _tileTypes;
public:
    bool empty;
    TileMap();
    ~TileMap();
    void Step();
    void Print();
    bool Fire(int pX, int pY);

    void MakeNew(int pMapSize);
    bool LoadFromFile();
    bool LoadFromServer();
    bool SaveToFile();
    bool SaveToServer();
    void Clear();
};


#endif //WILDFIRESIMULATOR_TILEMAP_H
