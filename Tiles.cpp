//
// Created by tomas on 2. 1. 2024.
//

#include <iostream>
#include "Tiles.h"

using namespace std;

void TileType::Update(Tile *pCurrent, int pWindDirection, Tile *pNorth, Tile *pEast, Tile *pSouth, Tile *pWest,
                      int *outNewType, bool *outOnFire) {
    Tile *surrounding[4];
    surrounding[0] = pNorth;
    surrounding[1] = pEast;
    surrounding[2] = pSouth;
    surrounding[3] = pWest;

    *outNewType = pCurrent->_type;
    *outOnFire = false;

    auto unif = uniform_real_distribution(0.0, 1.0);
    auto re = default_random_engine();

    for (int i = 0; i < 4; ++i) {

        //zapalenie biotopu
        if (!pCurrent->_onFire) {
            double fireChance = 0;

            if (surrounding[i] != nullptr && surrounding[i]->_onFire) {
                if (pWindDirection == i && pWindDirection >= 0) {
                    fireChance = _fireAgainstWind;
                } else if ((pWindDirection + 2) %4 == i && pWindDirection >= 0) {
                    fireChance = _fireWind;
                } else {
                    fireChance = _fireNoWind;
                }
                cout << endl << fireChance << endl;
            }

            if (abs(fireChance) >= 0.001 && unif(re) <= fireChance) {
                *outOnFire = true;
            }
        } else {
            *outOnFire = true;
        }

        //premena biotopu
        if (pCurrent->_onFire) {
            *outNewType = 0;
        } else {

        }
    }
}

TileType TileType::getBurntTileType() {
    TileType tileType;
    tileType._type = 0;
    tileType._transforms.push_back(TileTransformation{
            ._transformChance = 0.1,
            ._transformTile = 2,
            ._requiredTile = 4
    });
    return tileType;
}

TileType TileType::getForestTileType() {
    TileType tileType;
    tileType._type = 1;
    tileType._fireWind = 0.9;
    tileType._fireNoWind = 0.2;
    tileType._fireAgainstWind = 0.02;
    return tileType;
}

TileType TileType::getGrassTileType() {
    TileType tileType;
    tileType._type = 2;
    tileType._fireWind = 0.9;
    tileType._fireNoWind = 0.2;
    tileType._fireAgainstWind = 0.02;
    tileType._transforms.push_back(TileTransformation{
            ._transformChance = 0.02,
            ._transformTile = 1,
            ._requiredTile = 1
    });
    return tileType;
}

TileType TileType::getRocksTileType() {
    TileType tileType;
    tileType._type = 3;
    return tileType;
}

TileType TileType::getWaterTileType() {
    TileType tileType;
    tileType._type = 4;
    return tileType;
}

TileType::TileType() {
    _type = 1;
}

TileType::~TileType() {
    _transforms.clear();
}