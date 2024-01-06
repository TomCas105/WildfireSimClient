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
    for (int i = 0; i < _mapSize; ++i) {
        _mapCurrent[i].clear();
        _mapLast[i].clear();
    }
    _mapCurrent.clear();
    _mapLast.clear();
}

struct FireData {
    Map _map;
    vector<int> _xCords;
    vector<int> _yCords;
};

void *enflameTile(void *arg) {
    auto *data = static_cast<FireData *>(arg);
    auto map = data->_map;

    int i = 20;
    while (i > 0) {
        i--;

        //Pristup k mape
        pthread_mutex_lock(&map._mut);

        //Cakanie ak su suradnice pre ohen prazdne
        while (data->_xCords.empty()) {
            //Signal pre krok simulacie
            pthread_cond_signal(&map._cond_step);
            pthread_cond_wait(&map._cond_fire, &map._mut);
        }

        //Zapalenie biotopov
        for (int i = 0; i < data->_xCords.size(); ++i) {
            map._mapCurrent[data->_xCords[i]][data->_yCords[i]]._fireDuration = 3;
        }

        //Uvolnenie mapy
        pthread_mutex_unlock(&map._mut);
    }
    return nullptr;
}

struct MapData {
    Map _map;
    vector<TileType> _tileTypes;
};

void *mapStep(void *arg) {
    auto *data = static_cast<MapData *>(arg);

    auto map = data->_map;

    int windDirection = 0;
    int windDuration = 0;

    int i = 20;
    while (i > 0) {
        i--;

        //Pristup k mape
        pthread_mutex_lock(&map._mut);

        pthread_cond_wait(&map._cond_step, &map._mut);

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

        //buffering
        for (int y = 0; y < map._mapSize; y++) {
            for (int x = 0; x < map._mapSize; x++) {
                map._mapLast[x][y]._type = map._mapCurrent[x][y]._type;
                map._mapLast[x][y]._fireDuration = map._mapCurrent[x][y]._fireDuration;
            }
        }

        //krok
        for (int y = 0; y < map._mapSize; y++) {
            for (int x = 0; x < map._mapSize; x++) {
                Tile *current = &map._mapLast[x][y];
                Tile *north = nullptr;
                Tile *east = nullptr;
                Tile *south = nullptr;
                Tile *west = nullptr;

                if (y < map._mapSize - 1) {
                    north = &map._mapLast[x][y + 1];
                }
                if (x < map._mapSize - 1) {
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

                data->_tileTypes[current->_type].Update(
                        current, windDirection, north, east, south, west,
                        &newType, &fireDuration
                );

                map._mapCurrent[x][y]._type = newType;
                map._mapCurrent[x][y]._fireDuration = fireDuration;
            }
        }

        //vypis
        for (int y = 0; y < map._mapSize; y++) {
            for (int x = 0; x < map._mapSize; x++) {
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
            printf("\n\n");
        }
        string wind = "Bezvetrie";
        switch (windDirection) {
            case 0:
                wind = "Sever";
                break;
            case 1:
                wind = "Vychod";
                break;
            case 2:
                wind = "Juh";
                break;
            case 3:
                wind = "Zapad";
                break;
            default:
                break;
        }
        printf("Vietor %s\n\n\n", wind.c_str());

        //Signal pre vytvorenie ohna
        pthread_cond_signal(&map._cond_fire);

        //Uvolnenie mapy
        pthread_mutex_unlock(&map._mut);

        Sleep(1000);
    }
    return nullptr;
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

    //Init mutexu a kond. premennych
    pthread_mutex_init(&map._mut, nullptr);
    pthread_cond_init(&map._cond_step, nullptr);
    pthread_cond_init(&map._cond_fire, nullptr);

    //Vytvorenie vlakna simulacie
    pthread_t simulationThread;
    MapData mapData = {
            ._map = map,
            ._tileTypes = tileTypes
    };
    pthread_create(&simulationThread, nullptr, mapStep, &mapData);

    //Vytvorenie vlakna pre ohen
    pthread_t fireThread;
    FireData fireData = {
            ._map = map
    };
    pthread_create(&fireThread, nullptr, enflameTile, &fireData);

    cout << "Prikazy: \nfire [x] [y] - zapalenie policka na XY suradniciach" << endl;
    string input;
    cin >> input;
    while (input != "exit") {
        //Prikazy
        if (input == "fire") {
            try {
                cin >> input;
                int x = stoi(input);
                cin >> input;
                int y = stoi(input);

                fireData._xCords.push_back(x);
                fireData._xCords.push_back(y);

                //Signal pre vytvorenie ohna
                pthread_cond_signal(&map._cond_fire);
            }
            catch (exception &e) {
                printf("Neplatny prikaz.");
            }
        }


        cin >> input;
    }

    //Join vlakien
    pthread_join(simulationThread, nullptr);
    pthread_join(fireThread, nullptr);

    //Zanik mutexu a kond. premenych
    pthread_mutex_destroy(&map._mut);
    pthread_cond_destroy(&map._cond_step);
    pthread_cond_destroy(&map._cond_fire);

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