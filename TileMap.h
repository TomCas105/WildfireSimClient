//
// Created by tomas on 8. 1. 2024.
//
#pragma once
#include <iostream>
#include "Tiles.h"
//include "color.hpp"

#ifndef WILDFIRESIMULATOR_TILEMAP_H
#define WILDFIRESIMULATOR_TILEMAP_H

using namespace std;

class TileMap {
private:
    vector<vector<Tile>> _map;
    vector<vector<Tile>> _mapBuffer;
    int _mapSize;
    int _windDirection;
    int _windDuration;
    vector<TileType> _tileTypes;
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
