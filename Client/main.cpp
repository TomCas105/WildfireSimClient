
#include <iostream>
#include <fstream>
#include <sstream>
#include "TileMap.h"

using namespace std;

int main() {
    //Socket init
    ClientSocket* clientSocket = ClientSocket::createConnection("frios2.fri.uniza.sk", 11889);

    srand(time(nullptr));

    //Map init
    TileMap tileMap(clientSocket);

    string command;
    cout <<
         "[new] pre vytvorenie novej mapy, [load] pre nacitanie mapy, [end] ukonci"
         << endl;

    bool run = true;

    if (command == "new") {
        int mapSize = 10;
        cout << "Zadajte velkost mapy: " << endl;
        cin >> mapSize;
        tileMap.MakeNew(mapSize);
    } else if (command == "load") {
        tileMap.LoadFromServer();
    } else if (command == "end") {
        run = false;
    }

    while (run) {
        system("cls");
        tileMap.Step();
        tileMap.Print();

        cout << endl;
        cout <<
             "*ENTER* krok simulacie, [fire] [x] [y] zapalenie biotopu, [save] ulozi mapu, [load] nacita mapu, [end] ukonci"
             << endl;

        getline(cin, command);
        cin.clear();
        if (command == "end") {
            run = false;
        } else if (command.substr(0, 4) == "fire") {
            int x, y;
            istringstream iss(command.substr(5));
            if (iss >> x >> y) {
                if (tileMap.Fire(x-1, y-1)) {
                    cout << "Zapal na pozicii: [" << x << ", " << y << "]" << endl;
                } else {
                    cout << "Tento typ biotopu nemoze horiet" << endl;
                }
            } else {
                cout << "Neznamy prikaz" << endl;
            }
            getline(cin, command);
        } else if (command == "save") {
            tileMap.SaveToServer();
        } else if (command == "load") {
            tileMap.LoadFromServer();
        } else {
            cout << "Neznamy prikaz" << endl;
        }
    }

    delete clientSocket;
    clientSocket = nullptr;

    return 0;
}