#include <random>
#include <iostream>
#include <fstream>
#include <limits>
#include "color.hpp"
#include "Tiles.h"

using namespace std;

int main() {
    int mapSize = 10;
    cout << "Zadajte velkost mapy: ";
    cin >> mapSize;


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
    tileMap[5][2]._fireDuration = 3;
    tileMap[5][3]._fireDuration = 3;
    tileMap[5][4]._fireDuration = 3;
    tileMap[6][4]._fireDuration = 3;


    int windDirection = 0;
    int windDuration = 0;

    while (true) {
        //prekopirovanie
        for (int y = 0; y < mapSize; y++) {
            for (int x = 0; x < mapSize; x++) {
                tileMapLast[x][y]._type = tileMap[x][y]._type;
                tileMapLast[x][y]._fireDuration = tileMap[x][y]._fireDuration;
            }
        }

        if (windDuration != 0) {
            double p = (double) rand() / RAND_MAX;
            if (p >= 0.25) {
                windDuration--;
            }
        } else {
            double p = (double) rand() / RAND_MAX;
            if (p <= 0.1) {
                windDirection = rand() % 3;
                windDuration = 2;
            } else {
                windDirection = -1;
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
                    east = &tileMapLast[x + 1][y];
                }
                if (y > 0) {
                    south = &tileMapLast[x][y - 1];
                }
                if (x > 0) {
                    west = &tileMapLast[x - 1][y];
                }

                int fireDuration = false;
                int newType = current->_type;

                tileTypes[current->_type].Update(
                        current, windDirection, north, east, south, west,
                        &newType, &fireDuration
                );

                tileMap[x][y]._type = newType;
                tileMap[x][y]._fireDuration = fireDuration;
            }
        }

        //vypis
        for (int y = 0; y < mapSize; y++) {
            for (int x = 0; x < mapSize; x++) {
                switch (tileMap[x][y]._type) {
                    case 0:
                        if (tileMap[x][y]._fireDuration > 0) {
                            cout << dye::colorize("B", "red").invert();
                        } else {
                            cout << dye::light_red("B");
                        }
                        break;
                    case 1:
                        if (tileMap[x][y]._fireDuration > 0) {
                            cout << dye::colorize("F", "red").invert();
                        } else {
                            cout << dye::green("F");
                        }
                        break;
                    case 2:
                        if (tileMap[x][y]._fireDuration > 0) {
                            cout << dye::colorize("G", "red").invert();
                        } else {
                            cout << dye::light_green("G");
                        }
                        break;
                    case 3:
                        cout << dye::grey("R");
                        break;
                    default:
                        cout << dye::blue("W");
                        break;
                }
                cout << "   ";
            }
            cout << endl << endl;
        }
        cout << "Vietor: "
             << (windDirection == 0 ? "Sever" : windDirection == 1 ? "Vychod" : windDirection == 2 ? "Juh" :
                                                                                windDirection == 3 ? "Zapad"
                                                                                                   : "Bezvetrie");
        cout << endl << endl << endl;

        cin.clear();
        string moznost;
        cout << "[hocico] pokracuj, [end] ukonci, [fire] zapal ";
        cin >> moznost;

        if (moznost == "end") {
            break;
        }

        if (moznost == "fire") {
            try {
                cout << "suradnica X: ";
                int x;
                cin >> x;
                cout << "suradnica Y: ";
                int y;
                cin >> y;
            }
            catch (exception &e) {
                cerr << "somarina, my sme poobede" << endl;
            }
        }
        cout << endl;
    }

    std::ofstream file("data.txt");
    if (file.is_open()) {
        file << mapSize << "\n";
        for (int y = 0; y < mapSize; y++) {
            for (int x = 0; x < mapSize; x++) {
                file << tileMap[x][y]._type << " " << tileMap[x][y]._fireDuration << " ";
            }
            file << "\n";
        }
        file.close();
        cout << "Mapa je ulozena" << endl;
    } else {
        std::cerr << "Nepodarilo sa otvorit subor pre zapis." << std::endl;
        return 1;
    }

    return 0;
}