
#include <random>
#include <iostream>
#include "Tiles.h"

using namespace std;

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
    tileMap[5][2]._onFire = true;
    tileMap[5][3]._onFire = true;
    tileMap[5][4]._onFire = true;
    tileMap[6][4]._onFire = true;


    int windDirection = 1;

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
                Tile *current = &tileMapLast[x][y];
                Tile *north = nullptr;
                Tile *east = nullptr;
                Tile *south = nullptr;
                Tile *west = nullptr;

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

                bool onFire = false;
                int newType = current->_type;

                tileTypes[current->_type].Update(
                        current, windDirection, north, east, south, west,
                        &newType, &onFire
                );

                tileMap[x][y]._type = newType;
                tileMap[x][y]._onFire = onFire;
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
        cout << "Vietor: "
             << (windDirection == 0 ? "Sever" : windDirection == 1 ? "Vychod" : windDirection == 2 ? "Juh" :
                                                                                windDirection == 0 ? "Zapad"
                                                                                                   : "Bezvetrie");
        cout << endl << endl << endl;
        Sleep(2000);
    }

    return 0;
}
