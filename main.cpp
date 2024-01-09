#include <iostream>
#include <fstream>
#include <sstream>
#include <pthread.h>
#include <unistd.h>
#include "Tiles.h"
#include "color.hpp"
#include "ClientSocket.h"

using namespace std;

struct Map {
    std::vector<std::vector<Tile>> _map;
    std::vector<std::vector<Tile>> _mapLast;
    int _mapSize;
    int _windDirection;
    int _windDuration;
    std::vector<TileType> _tileTypes;
    bool simulationEnd;
    ClientSocket *clientSocket;
    pthread_mutex_t mutex;
};

void* print(void* arg) {
    Map &map = *static_cast<Map*>(arg);

    while (!map.simulationEnd) {
        pthread_mutex_lock(&map.mutex);
        for (int y = map._mapSize - 1; y >= 0; y--) {
            printf("%2d ", y + 1);
            for (int x = 0; x < map._mapSize; x++) {
                switch (map._map[x][y]._type) {
                    case 0:
                        if (map._map[x][y]._fireDuration > 0) {
                            cout << dye::colorize("B", "red").invert();
                        } else {
                            cout << dye::light_red("B");
                        }
                        break;
                    case 1:
                        if (map._map[x][y]._fireDuration > 0) {
                            cout << dye::colorize("F", "red").invert();
                        } else {
                            cout << dye::green("F");
                        }
                        break;
                    case 2:
                        if (map._map[x][y]._fireDuration > 0) {
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
                cout << "  ";
            }
            cout << endl;
        }
        cout << " 0";
        for (int x = 0; x < map._mapSize; x++) {
            printf("%2d ", x + 1);
        }
        cout << endl;
        cout << "Vietor: "
             << (map._windDirection == 0 ? "Sever" :
                 map._windDirection == 1 ? "Vychod" :
                 map._windDirection == 2 ? "Juh" :
                 map._windDirection == 3 ? "Zapad" :
                 "Bezvetrie") << endl;
        pthread_mutex_unlock(&map.mutex);
        usleep(1000000);
    }
    return nullptr;
}

void* step(void* arg) {
    Map &map = *static_cast<Map*>(arg);

    while (!map.simulationEnd) {
        pthread_mutex_lock(&map.mutex);
        //buffering
        for (int y = 0; y < map._mapSize; y++) {
            for (int x = 0; x < map._mapSize; x++) {
                map._mapLast[x][y]._type = map._map[x][y]._type;
                map._mapLast[x][y]._fireDuration = map._map[x][y]._fireDuration;
            }
        }

        //zmena vetra
        if (map._windDuration != 0) {
            double p = (double) rand() / RAND_MAX;
            if (p >= 0.25) {
                map._windDuration--;
            }
        } else {
            double p = (double) rand() / RAND_MAX;
            if (p <= 0.1) {
                map._windDirection = rand() % 3;
                map._windDuration = 2;
            } else {
                map._windDirection = -1;
            }
        }

        //krok simulacie
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

                map._tileTypes[current->_type].Update(
                        current, map._windDirection, north, east, south, west,
                        &newType, &fireDuration
                );

                map._map[x][y]._type = newType;
                map._map[x][y]._fireDuration = fireDuration;
            }
        }
        pthread_mutex_unlock(&map.mutex);
        usleep(1000000);
    }
    return nullptr;
}

void loadMapFromFile(const std::string& filename, Map& map) {
    ifstream input(filename);
    if (!input.is_open()) {
        cout << "Chyba pri nacitavani lokalneho suboru";
        return;
    }

    string inputString;
    while (getline(input, inputString));

    if (!map._map.empty()) {
        for (int y = 0; y < map._mapSize; ++y) {
            map._map[y].clear();
            map._mapLast[y].clear();
        }
    }
    map._map.clear();
    map._mapLast.clear();

    istringstream iss(inputString);
    try {
        string input;
        iss >> input;
        map._mapSize = stoi(input);
        iss >> input;
        map._windDirection = stoi(input);
        iss >> input;
        map._windDuration = stoi(input);

        for (int y = 0; y < map._mapSize; ++y) {
            vector<Tile> row1;
            vector<Tile> row2;
            for (int x = 0; x < map._mapSize; ++x) {
                int type = 0;
                int fireDuration = 0;

                iss >> input;
                type = stoi(input);
                iss >> input;
                fireDuration = stoi(input);

                row1.push_back(Tile{
                                       ._type = type,
                                       ._fireDuration = fireDuration
                               }
                );
                row2.push_back(Tile{
                                       ._type = type,
                                       ._fireDuration = fireDuration
                               }
                );
            }
            map._map.push_back(row1);
            map._mapLast.push_back(row2);
        }
    } catch (exception &e) {
        cerr << "Chyba pri deserializovani." << endl;
        if (!map._map.empty()) {
            for (int y = 0; y < map._mapSize; ++y) {
                map._map[y].clear();
                map._mapLast[y].clear();
            }
        }
        map._map.clear();
        map._mapLast.clear();
    }
}

void loadMapFromServer(Map& map) {
    if (map.clientSocket != nullptr) {
        map.clientSocket->sendData("load");
        string output = map.clientSocket->receiveData();
        cout << output;
        istringstream iss(output);
        try {
            string input;
            iss >> input;
            map._mapSize = stoi(input);
            iss >> input;
            map._windDirection = stoi(input);
            iss >> input;
            map._windDuration = stoi(input);

            for (int y = 0; y < map._mapSize; ++y) {
                vector<Tile> row1;
                vector<Tile> row2;
                for (int x = 0; x < map._mapSize; ++x) {
                    int type = 0;
                    int fireDuration = 0;

                    iss >> input;
                    type = stoi(input);
                    iss >> input;
                    fireDuration = stoi(input);

                    row1.push_back(Tile{
                                           ._type = type,
                                           ._fireDuration = fireDuration
                                   }
                    );
                    row2.push_back(Tile{
                                           ._type = type,
                                           ._fireDuration = fireDuration
                                   }
                    );
                }
                map._map.push_back(row1);
                map._mapLast.push_back(row2);
            }
        } catch (exception &e) {
            cerr << "Chyba pri deserializovani." << endl;
            if (!map._map.empty()) {
                for (int y = 0; y < map._mapSize; ++y) {
                    map._map[y].clear();
                    map._mapLast[y].clear();
                }
            }
            map._map.clear();
            map._mapLast.clear();
        }
    }
}

void* autoLocalSave(void* arg) {
    Map &map = *static_cast<Map *>(arg);

    while (!map.simulationEnd) {
        pthread_mutex_lock(&map.mutex);
        ofstream oOutput("data.txt");
        string output = to_string(map._mapSize);
        output.append(" " + to_string(map._windDirection));
        output.append(" " + to_string(map._windDuration));
        for (int y = 0; y < map._mapSize; ++y) {
            for (int x = 0; x < map._mapSize; ++x) {
                output.append(" " + to_string(map._map[x][y]._type));
                output.append(" " + to_string(map._map[x][y]._fireDuration));
            }
        }
        oOutput << output;
        oOutput.close();
        pthread_mutex_unlock(&map.mutex);
        usleep(30000000);
    }
    return nullptr;
}

void* getCommand(void* arg) {
    Map &map = *static_cast<Map *>(arg);

    while (!map.simulationEnd) {
        usleep(1000000);
        // Overenie, či užívateľ stlačil Enter
        if (std::cin.peek() == '\n') {
            std::cin.ignore(); // Ignoruje stlačený Enter

            pthread_mutex_lock(&map.mutex);
            system("cls");
            string command;

            cout << endl;
            cout <<
                 "*ENTER* - krok simulacie\nfire [x] [y] - zapalenie biotopu\nsave [local/server] - ulozi mapu\nload [local/server] - nacita mapu\nend - ukonci"
                 << endl;

            getline(cin, command);
            cin.clear();
            if (command == "end") {
                map.simulationEnd = true;
            } else if (command.substr(0, 4) == "fire") {
                  int x, y;
                  istringstream iss(command.substr(5));
                  if (iss >> x >> y) {
                      if (x - 1 < 0 || x - 1 >= map._mapSize || y - 1 < 0 || y - 1 >= map._mapSize) {
                          cout << "Tento typ biotopu nemoze horiet" << endl;
                      } else if (map._tileTypes[map._map[x - 1][y - 1]._type].Flammable()) {
                          map._map[x - 1][y - 1]._fireDuration = 3;
                          cout << "Zapal na pozicii: [" << x << ", " << y << "]" << endl;
                      } else {
                          cout << "Tento typ biotopu nemoze horiet" << endl;
                      }
                  } else {
                      cout << "Neznamy prikaz" << endl;
                 }
                  getline(cin, command);
            } else if (command.substr(0, 10) == "save local") {
                ofstream oOutput("data.txt");
                string output = to_string(map._mapSize);
                output.append(" " + to_string(map._windDirection));
                output.append(" " + to_string(map._windDuration));
                for (int y = 0; y < map._mapSize; ++y) {
                    for (int x = 0; x < map._mapSize; ++x) {
                        output.append(" " + to_string(map._map[x][y]._type));
                        output.append(" " + to_string(map._map[x][y]._fireDuration));
                    }
                }
                oOutput << output;
                oOutput.close();
                cout << "Data ulozene lokalne!" << endl;
            } else if (command.substr(0, 11) == "save server") {
                if (map.clientSocket != nullptr) {
                    string oOutput = to_string(map._mapSize);
                    oOutput.append(" " + to_string(map._windDirection));
                    oOutput.append(" " + to_string(map._windDuration));
                    for (int y = 0; y < map._mapSize; ++y) {
                        for (int x = 0; x < map._mapSize; ++x) {
                            oOutput.append(" " + to_string(map._map[x][y]._type));
                            oOutput.append(" " + to_string(map._map[x][y]._fireDuration));
                        }
                    }
                    string output = "save " + oOutput;
                    map.clientSocket->sendData(output);
                }
            } else if (command.substr(0, 10) == "load local") {
                loadMapFromFile("data.txt", map);
            } else if (command.substr(0, 11) == "load server") {
                loadMapFromServer(map);
            }
            pthread_mutex_unlock(&map.mutex);
        }
    }
    return nullptr;
}


int main() {
    srand(time(nullptr));
    Map map = {
            ._mapSize = 10,
            ._windDirection = 0,
            ._windDuration = 0,
            .simulationEnd = false,
    };

    //Socket init
    map.clientSocket = ClientSocket::createConnection("frios2.fri.uniza.sk", 11887);

    string command;
    cout <<
         "new - vytvorenie novej mapy\nload [local/server] - nacitanie mapy\nend - ukonci"
         << endl;

    getline(cin, command);
    cin.clear();

    if (command == "new") {
        int mapSize = 0;
        cout << "Zadajte velkost mapy: " << endl;
        cin >> mapSize;
        for (int y = 0; y < map._mapSize; ++y) {
            vector<Tile> row1;
            vector<Tile> row2;
            for (int x = 0; x < map._mapSize; ++x) {
                int type = 1 + rand() % 4;
                row1.push_back(Tile{
                                       ._type = type,
                                       ._fireDuration = 0
                               }
                );
                row2.push_back(Tile{
                                       ._type = type,
                                       ._fireDuration = 0
                               }
                );
            }
            map._map.push_back(row1);
            map._mapLast.push_back(row2);
        }
    } else if (command.substr(0, 10) == "load local") {
        loadMapFromFile("data.txt", map);
    } else if (command.substr(0, 11) == "load server") {
        loadMapFromServer(map);
    } else if (command == "end") {
        map.simulationEnd = true;
    }

    map._mapSize = 10;
    map._windDirection = -1;
    map._windDuration = 0;
    map._tileTypes.push_back(TileType::getBurntTileType());
    map._tileTypes.push_back(TileType::getForestTileType());
    map._tileTypes.push_back(TileType::getGrassTileType());
    map._tileTypes.push_back(TileType::getRocksTileType());
    map._tileTypes.push_back(TileType::getWaterTileType());

    pthread_mutex_init(&map.mutex, nullptr);

    pthread_t printVlakno;
    pthread_t stepVlakno;
    pthread_t autoLocalSaveVlakno;
    pthread_t getCommandVlakno;

    pthread_create(&printVlakno, nullptr, print, &map);
    pthread_create(&stepVlakno, nullptr, step, &map);
    pthread_create(&autoLocalSaveVlakno, nullptr, autoLocalSave, &map);
    pthread_create(&getCommandVlakno, nullptr, getCommand, &map);

    pthread_join(printVlakno, nullptr);
    pthread_join(stepVlakno, nullptr);
    pthread_join(autoLocalSaveVlakno, nullptr);
    pthread_join(getCommandVlakno, nullptr);

    pthread_mutex_destroy(&map.mutex);
    return 0;
}