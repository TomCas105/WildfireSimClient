//
// Created by tomas on 8. 1. 2024.
//
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "ClientSocket.h"
#include <windows.h>
#include "Tiles.h"
#include <string>
#include <queue>

#ifndef WILDFIRESIMULATOR_TILEMAP_H
#define WILDFIRESIMULATOR_TILEMAP_H

class TileMap {
private:
    std::vector<std::vector<Tile>> _map;
    std::vector<std::vector<Tile>> _mapLast;
    int _mapSize;
    int _windDirection;
    int _windDuration;
    std::vector<TileType> _tileTypes;

    ClientSocket* _socket;
    std::mutex _mutex;
    std::condition_variable _waitStep;
    std::condition_variable _waitLoad;
    std::condition_variable _waitSave;

public:
    bool empty;
    TileMap(ClientSocket *pSocket);
    ~TileMap();
    void MakeNew(int pMapSize);
    void Step();
    void Print();
    bool Fire(int pX, int pY);
    bool LoadFromServer();
    bool SaveToServer();
    bool LoadFromFile();
    void SaveToFile();
    void Clear();

    std::string Serialize();
    void Deserialize(std::string& input);
};


#endif //WILDFIRESIMULATOR_TILEMAP_H
