#include <iostream>
#include <vector>
#include <synchapi.h>
#include <random>

using namespace std;

struct TileTransformation {
    double _transformChance;
    int _transformTile;
    int _requiredTile;
};

struct Tile {
    int _type;
    bool _onFire;
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
           int *outNewType, bool *outOnFire) {
        Tile *surrounding[4];
        surrounding[0] = pNorth;
        surrounding[1] = pEast;
        surrounding[2] = pSouth;
        surrounding[3] = pWest;

        //zmena na zhoreny biotop
        if (pCurrent->_onFire) {
            *outNewType = 0;
        }

        auto unif = uniform_real_distribution(0.0, 1.0);
        auto re = default_random_engine(time(nullptr));

        for (int i = 0; i < 4; ++i) {

            //zapalenie biotopu
            if (!pCurrent->_onFire) {
                double fireChance = 0;

                if (surrounding[i] != nullptr && surrounding[i]->_onFire) {
                    if (pWindDirection == i) {
                        fireChance = _fireAgainstWind;
                    } else if (pWindDirection == 3 - i) {
                        fireChance = _fireWind;
                    } else {
                        fireChance = _fireNoWind;
                    }
                }

                if (abs(fireChance) >= 0.001 && unif(re) <= fireChance) {
                    *outOnFire = true;
                }
            }

            //premena biotopu
        }

        *outNewType = pCurrent->_type;
    }


    static TileType getBurntTileType() {
        TileType tileType;
        tileType._type = 0;
        tileType._transforms.push_back(TileTransformation{
                ._transformChance = 0.1,
                ._transformTile = 2,
                ._requiredTile = 4
        });
        return tileType;
    }

    static TileType getForestTileType() {
        TileType tileType;
        tileType._type = 1;
        tileType._fireWind = 0.9;
        tileType._fireNoWind = 0.2;
        tileType._fireAgainstWind = 0.02;
        return tileType;
    }

    static TileType getGrassTileType() {
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

    static TileType getRocksTileType() {
        TileType tileType;
        tileType._type = 3;
        return tileType;
    }

    static TileType getWaterTileType() {
        TileType tileType;
        tileType._type = 4;
        return tileType;
    }
};

TileType::TileType() {
    _type = 1;
}

TileType::~TileType() {
    _transforms.clear();
}

//0 zhorena, les 1, luka 2, skaly 3, voda 4
//vietor 0 sever, 1 vychod, 2 juh, 3 zapad

int main() {
    int mapSize = 10;
    srand(time(nullptr));

    //Tile type init
    TileType tileTypes[5];
    tileTypes[0] = TileType::getBurntTileType();
    tileTypes[1] = TileType::getForestTileType();
    tileTypes[2] = TileType::getGrassTileType();
    tileTypes[3] = TileType::getRocksTileType();
    tileTypes[4] = TileType::getWaterTileType();

    //Tile map init
    Tile tileMap[mapSize][mapSize];
    Tile tileMapLast[mapSize][mapSize];

    for (int y = 0; y < mapSize; y++) {
        for (int x = 0; x < mapSize; x++) {
            tileMap[x][y]._type = 1 + rand() % 4;
        }
    }

    int windDirection = -1;

    while (true) {
        //prekopirovanie
        for (int y = 0; y < mapSize; y++) {
            for (int x = 0; x < mapSize; x++) {
                tileMapLast[x][y]._type = tileMap[x][y]._type;
                tileMapLast[x][y]._onFire = tileMap[x][y]._onFire;
            }
        }

        for (int y = 0; y < mapSize; y++) {
            for (int x = 0; x < mapSize; x++) {
                bool onFire;
                int newType;

                Tile *current = &tileMapLast[x][y];
                Tile *north;
                Tile *east;
                Tile *south;
                Tile *west;

                if (y < mapSize - 1) {
                    north = &tileMapLast[x][y + 1];
                }
                if (x < mapSize - 1) {
                    east == &tileMapLast[x + 1][y];
                }
                if (y > 0) {
                    south = &tileMapLast[x][y - 1];
                }
                if (x > 0) {
                    west == &tileMapLast[x - 1][y];
                }
                tileTypes[current->_type].Update(
                        current, windDirection, north, east, south, west,
                        &newType, &onFire
                );
            }
        }

        //vypis
        for (int y = 0; y < mapSize; y++) {
            for (int x = 0; x < mapSize; x++) {
                cout << tileMap[x][y]._type;
                if (tileMap[x][y]._onFire) {
                    cout << "H   ";
                } else {
                    cout << "    ";
                }
            }
            cout << endl << endl;
        }
        Sleep(1000);
    }

    return 0;
}
