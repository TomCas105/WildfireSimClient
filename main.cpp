#include <random>
#include <iostream>
#include <pthread.h>
#include "color.hpp"
#include "Tiles.h"

using namespace std;

struct Map {
    int _mapSize;
    vector<vector<Tile>> _mapCurrent;
    vector<vector<Tile>> _mapLast;

    pthread_mutex_t _mut;
    pthread_cond_t _cond_step;
    pthread_cond_t _cond_fire;

    Map(int pSize);

    ~Map();
};

Map::Map(int pSize) {
    _mapSize = pSize;

    for (int y = 0; y < _mapSize; ++y) {
        vector<Tile> rowsC;
        vector<Tile> rowsL;
        for (int x = 0; x < _mapSize; ++x) {
            rowsC.push_back(Tile());
            rowsC[x]._type = 1 + rand() % 4;

            rowsL.push_back(Tile());
            rowsL[x]._type = rowsC[x]._type;
        }
        _mapCurrent.push_back(rowsC);
        _mapLast.push_back(rowsL);
    }
}

Map::~Map() {
    _mapCurrent.clear();
    _mapLast.clear();
}

struct EnflameTileData {
    Map _map;
    int _tileX;
    int _tileY;
};

void *enflameTile(void *arg) {
    auto *data = static_cast<EnflameTileData *>(arg);

}

struct MapStepData {
    Map _map;
    vector<TileType> _tileTypes;
};

void *mapStep(void *arg) {
    auto *data = static_cast<MapStepData *>(arg);

}

int main() {
    int mapSize = 10;

    srand(time(nullptr));

    //Tile type init
    vector<TileType> tileTypes;
    tileTypes.push_back(TileType::getBurntTileType());
    tileTypes.push_back(TileType::getForestTileType());
    tileTypes.push_back(TileType::getGrassTileType());
    tileTypes.push_back(TileType::getRocksTileType());
    tileTypes.push_back(TileType::getWaterTileType());

    //Tile map init
    Map map(mapSize);


    map._mapCurrent[5][2]._fireDuration = 3;
    map._mapCurrent[5][3]._fireDuration = 3;
    map._mapCurrent[6][3]._fireDuration = 3;
    map._mapCurrent[6][4]._fireDuration = 3;
    map._mapCurrent[7][4]._fireDuration = 3;


    int windDirection = 0;
    int windDuration = 0;

    while (true) {
        //buffering
        for (int y = 0; y < mapSize; y++) {
            for (int x = 0; x < mapSize; x++) {
                map._mapLast[x][y]._type = map._mapCurrent[x][y]._type;
                map._mapLast[x][y]._fireDuration = map._mapCurrent[x][y]._fireDuration;
            }
        }

        //vietor
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

        //krok
        for (int y = 0; y < mapSize; y++) {
            for (int x = 0; x < mapSize; x++) {
                Tile *current = &map._mapLast[x][y];
                Tile *north = nullptr;
                Tile *east = nullptr;
                Tile *south = nullptr;
                Tile *west = nullptr;

                if (y < mapSize - 1) {
                    north = &map._mapLast[x][y + 1];
                }
                if (x < mapSize - 1) {
                    east = &map._mapLast[x + 1][y];
                }
                if (y > 0) {
                    south = &map._mapLast[x][y - 1];
                }
                if (x > 0) {
                    west = &map._mapLast[x - 1][y];
                }

                int fireDuration = false;
                int newType = current->_type;

                tileTypes[current->_type].Update(
                        current, windDirection, north, east, south, west,
                        &newType, &fireDuration
                );

                map._mapCurrent[x][y]._type = newType;
                map._mapCurrent[x][y]._fireDuration = fireDuration;
            }
        }

        //vypis
        for (int y = 0; y < mapSize; y++) {
            for (int x = 0; x < mapSize; x++) {
                switch (map._mapCurrent[x][y]._type) {
                    case 0:
                        if (map._mapCurrent[x][y]._fireDuration > 0) {
                            cout << dye::colorize("B", "red").invert();
                        } else {
                            cout << dye::light_red("B");
                        }
                        break;
                    case 1:
                        if (map._mapCurrent[x][y]._fireDuration > 0) {
                            cout << dye::colorize("F", "red").invert();
                        } else {
                            cout << dye::green("F");
                        }
                        break;
                    case 2:
                        if (map._mapCurrent[x][y]._fireDuration > 0) {
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
                cout << "    ";
            }
            cout << endl << endl;
        }
        cout << "Vietor: "
             << (windDirection == 0 ? "Sever" : windDirection == 1 ? "Vychod" : windDirection == 2 ? "Juh" :
                                                                                windDirection == 3 ? "Zapad"
                                                                                                   : "Bezvetrie");
        cout << endl << endl << endl;
        Sleep(1000);
    }
    return 0;
}

/* Bordeľ

    bool newSimulation = false;
    cout << "Zadajte velkost mapy: ";
    cin >> mapSize;
    cin.ignore();
    cout << "Nova simulacia? ";
    cin >> newSimulation;
    cin.ignore();

    Tile tileMap[mapSize][mapSize];
    Tile tileMapLast[mapSize][mapSize];

if (newSimulation) {
        tileMap[5][2]._fireDuration = 3;
        tileMap[5][3]._fireDuration = 3;
        tileMap[5][4]._fireDuration = 3;
        tileMap[6][4]._fireDuration = 3;
    } else {
        ifstream file("data.txt");
        if (file.is_open()) {
            while (!file.eof()) {
                file >> mapSize;
                for (int y = 0; y < mapSize; y++) {
                    for (int x = 0; x < mapSize; x++) {
                        file >> tileMap[x][y]._type >> tileMap[x][y]._fireDuration;
                    }
                }
            }
            file.close();
        } else {
            std::cerr << "Nepodarilo sa otvorit subor pre citanie" << std::endl;
            return 1;
        }
        tileMap[0][0]._fireDuration = 3;
    }


        cin.clear();
        string moznost;
        cout << "[hocico] pokracuj, [end] ukonci, [fire] zapal ";
        cin >> moznost;

        if (moznost == "end") {
            break;
        }

        if (moznost == "fire") {
            cout << "dokoncit" << endl; //TODO
        }
        cout << endl;


    ofstream file("data.txt");
    if (file.is_open()) {
        file << mapSize << "\n";
        for (int y = 0; y < mapSize; y++) {
            for (int x = 0; x < mapSize; x++) {
                file << tileMap[x][y]._type << " " << tileMap[x][y]._fireDuration << " ";
            }
            file << "\n";
        }
        file.close();
        cout << "Map je ulozena" << endl;
    } else {
        cerr << "Nepodarilo sa otvorit subor pre zapis." << std::endl;
        return 1;
    }
 */