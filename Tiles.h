//
// Created by tomas on 2. 1. 2024.
//
#pragma once

#include <random>
#include <vector>
#include <synchapi.h>

using namespace std;

#ifndef WILDFIRESIMULATOR_TILES_H
#define WILDFIRESIMULATOR_TILES_H


struct TileTransformation {
    double _transformChance;
    int _transformTile;
    int _requiredTile;
};

struct Tile {
    int _type = 1;
    bool _onFire = false;
};

class TileType {
private:
    int _type;
    double _fireWind;
    double _fireNoWind;
    double _fireAgainstWind;
    vector<TileTransformation> _transforms;
public:
    TileType();

    ~TileType();

    void
    Update(Tile *pCurrent, int pWindDirection, Tile *pNorth, Tile *pEast, Tile *pSouth, Tile *pWest,
           int *outNewType, bool *outOnFire);


    static TileType getBurntTileType();

    static TileType getForestTileType();

    static TileType getGrassTileType();

    static TileType getRocksTileType();

    static TileType getWaterTileType();
};


#endif //WILDFIRESIMULATOR_TILES_H
