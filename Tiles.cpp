//
// Created by tomas on 2. 1. 2024.
//

#include <iostream>
#include "Tiles.h"

using namespace std;

void TileType::Update(Tile *pCurrent, int pWindDirection, Tile *pNorth, Tile *pEast, Tile *pSouth, Tile *pWest,
                      int *outNewType, int *outFireDuration) {
    Tile *surrounding[4];
    surrounding[0] = pNorth;
    surrounding[1] = pEast;
    surrounding[2] = pSouth;
    surrounding[3] = pWest;

    *outNewType = pCurrent->_type;
    *outFireDuration = pCurrent->_fireDuration;

    for (int i = 0; i < 4; ++i) {

        //zapalenie biotopu
        if (pCurrent->_fireDuration <= 0) {
            double fireChance = 0;

            if (surrounding[i] != nullptr && surrounding[i]->_fireDuration) {
                if (pWindDirection == i && pWindDirection >= 0) {
                    fireChance = _fireAgainstWind;
                } else if ((pWindDirection + 2) % 4 == i && pWindDirection >= 0) {
                    fireChance = _fireWind;
                } else {
                    fireChance = _fireNoWind;
                }
            }
            double p = (double) rand() / RAND_MAX;
            if (abs(fireChance) >= 0.001 && p <= fireChance) {
                *outFireDuration = 3;
            }
        } else {
            //znizenie casu horenia
            *outFireDuration = pCurrent->_fireDuration - 1;
        }

        //premena biotopu
        if (pCurrent->_fireDuration > 0) {
            *outNewType = 0;
        } else {
            if (surrounding[i] != nullptr) {
                for (int j = 0; j < _transforms.size(); j++) {
                    if (surrounding[i]->_type == _transforms[j]._requiredTile) {
                        double p = (double) rand() / RAND_MAX;
                        if (p < _transforms[j]._transformChance) {
                            *outNewType = _transforms[j]._transformTile;
                        }
                    }
                }
            }
        }
    }

}

TileType TileType::getBurntTileType() {
    TileType tileType;
    tileType._type = 0;
    tileType._fireWind = 0;
    tileType._fireNoWind = 0;
    tileType._fireAgainstWind = 0;
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
    tileType._fireWind = 0;
    tileType._fireNoWind = 0;
    tileType._fireAgainstWind = 0;
    return tileType;
}

TileType TileType::getWaterTileType() {
    TileType tileType;
    tileType._type = 4;
    tileType._fireWind = 0;
    tileType._fireNoWind = 0;
    tileType._fireAgainstWind = 0;
    return tileType;
}

TileType::TileType() {
    _type = 1;
}

TileType::~TileType() {
    _transforms.clear();
}