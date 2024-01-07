#include <random>
#include <iostream>
#include <fstream>
#include <sstream>
#include "color.hpp"
#include "Tiles.h"
#include "ClientSocket.h"

using namespace std;

int main() {
    srand(time(nullptr));

    int mapSize = 10;

    cout << "Zadajte velkost mapy: " << endl;
    cin >> mapSize;
    cin.ignore();
    cin.clear();

    //Socket init
    ClientSocket* clientSocket = ClientSocket::createConnection("frios2.fri.uniza.sk", 11889);

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
            tileMap[x][y]._fireDuration = 0;
        }
    }

    int windDirection = 0;
    int windDuration = 0;
    bool run = true;

    while (run) {
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
             << (windDirection == 0 ? "Sever" :
                 windDirection == 1 ? "Vychod" :
                 windDirection == 2 ? "Juh" :
                 windDirection == 3 ? "Zapad" : "Bezvetrie");

        //buffering
        for (int y = 0; y < mapSize; y++) {
            for (int x = 0; x < mapSize; x++) {
                tileMapLast[x][y]._type = tileMap[x][y]._type;
                tileMapLast[x][y]._fireDuration = tileMap[x][y]._fireDuration;
            }
        }

        //zmena vetra
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

        //krok simulacie
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

        cout << endl << endl << endl;

        cin.clear();
        cin.clear();
        cin.clear();

        string command;
        cout <<
             "*ENTER* krok simulacie, [fire] [x] [y] zapalenie biotopu, [save] ulozi mapu do suboru, [load] nacita mapu zo suboru, [end] ukonci"
             << endl;

        getline(cin, command);
        cin.clear();
        if (command == "end") {
            run = false;
        } else if (command.substr(0, 4) == "fire") {
            int x, y;
            istringstream iss(command.substr(5));
            if (iss >> x >> y) {
                if (!tileTypes[tileMap[y][x]._type].Flammable()) {
                    cout << "Tento typ biotopu nemoze horiet" << endl;
                } else {
                    cout << "Zapal na pozicii: [" << x << ", " << y << "]" << endl;
                    tileMap[y][x]._fireDuration = 3;
                }
            } else {
                cout << "Neznamy prikaz" << endl;
            }
        } else if (command == "save") {
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
                cout << "Mapa je ulozena lokalne" << endl;
            } else {
                cerr << "Nepodarilo sa otvorit subor pre lokalny zapis." << endl;
            }
        } else if (command == "load") {
            ifstream file("data.txt");
            if (file.is_open()) {
                while (!file.eof()) {
                    file >> mapSize;
                    // TODO treba osetrit velkost mapy
                    for (int y = 0; y < mapSize; y++) {
                        for (int x = 0; x < mapSize; x++) {
                            file >> tileMap[x][y]._type >> tileMap[x][y]._fireDuration;
                        }
                    }
                }
                file.close();
            } else {
                cerr << "Nepodarilo sa otvorit subor pre lokalne citanie" << endl;
            }
        } else {
            cout << "Neznamy prikaz" << endl;
        }
    }

    delete clientSocket;
    clientSocket = nullptr;

    return 0;
}