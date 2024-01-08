//
// Created by tomas on 2. 1. 2024.
//
#pragma once

#include <random>
#include <vector>
#include <synchapi.h>

#ifndef WILDFIRESIMULATOR_TILES_H
#define WILDFIRESIMULATOR_TILES_H


struct TileTransformation {
    double _transformChance;
    int _transformTile;
    int _requiredTile;
};

struct Tile {
    int _type = 1;
    int _fireDuration = 0;
};

class TileType {
private:
    int _type;
    double _fireWind;
    double _fireNoWind;
    double _fireAgainstWind;
    std::vector<TileTransformation> _transforms;
public:
    TileType();

    ~TileType();

    bool Flammable();

    void Update(Tile *pCurrent, int pWindDirection, Tile *pNorth, Tile *pEast, Tile *pSouth, Tile *pWest,
           int *outNewType, int *outFireDuration);


    static TileType getBurntTileType();

    static TileType getForestTileType();

    static TileType getGrassTileType();

    static TileType getRocksTileType();

    static TileType getWaterTileType();
};


#endif //WILDFIRESIMULATOR_TILES_H
