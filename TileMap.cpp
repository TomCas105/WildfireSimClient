//
// Created by tomas on 8. 1. 2024.
//

#include "TileMap.h"
#include "color.hpp"

using namespace std;

TileMap::TileMap(ClientSocket *pSocket) {
    empty = true;
    _socket = pSocket;
    _mapSize = 0;
    _windDirection = -1;
    _windDuration = 0;
    _tileTypes.push_back(TileType::getBurntTileType());
    _tileTypes.push_back(TileType::getForestTileType());
    _tileTypes.push_back(TileType::getGrassTileType());
    _tileTypes.push_back(TileType::getRocksTileType());
    _tileTypes.push_back(TileType::getWaterTileType());
}

TileMap::~TileMap() {
    if (_socket != nullptr) {
        _socket->sendData("exit");
        _socket->sendEndMessage();
    }
}

void TileMap::Step() {

    //buffering
    for (int y = 0; y < _mapSize; y++) {
        for (int x = 0; x < _mapSize; x++) {
            _mapLast[x][y]._type = _map[x][y]._type;
            _mapLast[x][y]._fireDuration = _map[x][y]._fireDuration;
        }
    }

    //zmena vetra
    if (_windDuration != 0) {
        double p = (double) rand() / RAND_MAX;
        if (p >= 0.25) {
            _windDuration--;
        }
    } else {
        double p = (double) rand() / RAND_MAX;
        if (p <= 0.1) {
            _windDirection = rand() % 3;
            _windDuration = 2;
        } else {
            _windDirection = -1;
        }
    }

    //krok simulacie
    for (int y = 0; y < _mapSize; y++) {
        for (int x = 0; x < _mapSize; x++) {
            Tile *current = &_mapLast[x][y];
            Tile *north = nullptr;
            Tile *east = nullptr;
            Tile *south = nullptr;
            Tile *west = nullptr;

            if (y < _mapSize - 1) {
                north = &_mapLast[x][y + 1];
            }
            if (x < _mapSize - 1) {
                east = &_mapLast[x + 1][y];
            }
            if (y > 0) {
                south = &_mapLast[x][y - 1];
            }
            if (x > 0) {
                west = &_mapLast[x - 1][y];
            }

            int fireDuration = false;
            int newType = current->_type;

            _tileTypes[current->_type].Update(
                    current, _windDirection, north, east, south, west,
                    &newType, &fireDuration
            );

            _map[x][y]._type = newType;
            _map[x][y]._fireDuration = fireDuration;
        }
    }
}

void TileMap::Print() {
    for (int y = _mapSize - 1; y >= 0; y--) {
        printf("%2d ", y + 1);
        for (int x = 0; x < _mapSize; x++) {
            switch (_map[x][y]._type) {
                case 0:
                    if (_map[x][y]._fireDuration > 0) {
                        cout << dye::colorize("B", "red").invert();
                    } else {
                        cout << dye::light_red("B");
                    }
                    break;
                case 1:
                    if (_map[x][y]._fireDuration > 0) {
                        cout << dye::colorize("F", "red").invert();
                    } else {
                        cout << dye::green("F");
                    }
                    break;
                case 2:
                    if (_map[x][y]._fireDuration > 0) {
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
    for (int x = 0; x < _mapSize; x++) {
        printf("%2d ", x + 1);
    }
    cout << endl;
    cout << "Vietor: "
         << (_windDirection == 0 ? "Sever" :
             _windDirection == 1 ? "Vychod" :
             _windDirection == 2 ? "Juh" :
             _windDirection == 3 ? "Zapad" :
             "Bezvetrie") << endl;

}

bool TileMap::Fire(int pX, int pY) {
    if (pX < 0 || pX >= _mapSize || pY < 0 || pY >= _mapSize) {
        return false;
    }
    if (_tileTypes[_map[pX][pY]._type].Flammable()) {
        _map[pX][pY]._fireDuration = 3;
        return true;
    }
    return false;
}

void TileMap::MakeNew(int pMapSize) {
    Clear();
    empty = false;
    _mapSize = pMapSize;
    _windDirection = -1;
    _windDuration = 0;

    for (int y = 0; y < _mapSize; ++y) {
        vector<Tile> row1;
        vector<Tile> row2;
        for (int x = 0; x < _mapSize; ++x) {
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
        _map.push_back(row1);
        _mapLast.push_back(row2);
    }
}

void TileMap::Clear() {
    if (!_map.empty()) {
        for (int y = 0; y < _mapSize; ++y) {
            _map[y].clear();
            _mapLast[y].clear();
        }
    }
    _map.clear();
    _mapLast.clear();
}

string TileMap::Serialize() {
    string output = to_string(_mapSize);
    output.append("\n" + to_string(_windDirection));
    output.append("\n" + to_string(_windDuration));
    for (int y = 0; y < _mapSize; ++y) {
        for (int x = 0; x < _mapSize; ++x) {
            output.append("\n" + to_string(_map[x][y]._type));
            output.append(" " + to_string(_map[x][y]._fireDuration));
        }
    }
    return output;
}

void TileMap::Deserialize(string& input) {
    Clear();
    istringstream iss(input);
    try {
        string input;
        iss >> input;
        _mapSize = stoi(input);
        iss >> input;
        _windDirection = stoi(input);
        iss >> input;
        _windDuration = stoi(input);

        for (int y = 0; y < _mapSize; ++y) {
            vector<Tile> row1;
            vector<Tile> row2;
            for (int x = 0; x < _mapSize; ++x) {
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
            _map.push_back(row1);
            _mapLast.push_back(row2);
        }
    } catch (exception& e) {
        cerr << "Chyba pri deserializovani." << endl;
        Clear();
        empty = true;
    }
}

bool TileMap::SaveToServer(string mapName) {
    if (_socket != nullptr) {
        _socket->sendData("save " + mapName);
        string output = Serialize();
        _socket->sendData(output);
        return true;
    }
    return false;
}

bool TileMap::LoadFromServer(string mapName) {
    if (_socket != nullptr) {
        _socket->sendData("load " + mapName);
        string output = _socket->receiveData();
        Deserialize(output);
    }
    return false;
}

void TileMap::SaveToFile(string mapName) {
    ofstream output(mapName);
    output << Serialize();
    output.close();
}

bool TileMap::LoadFromFile(string mapName) {
    ifstream input(mapName);
    string inputString;

    while (getline(input, inputString));

    Deserialize(inputString);

    return false;
}
