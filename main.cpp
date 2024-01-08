
#include <iostream>
#include <fstream>
#include <sstream>
#include "TileMap.h"

using namespace std;

int main() {
    //Socket init
    ClientSocket *clientSocket = ClientSocket::createConnection("frios2.fri.uniza.sk", 11887);

    srand(time(nullptr));

    //Map init
    TileMap tileMap(clientSocket);

    string command;
    cout <<
         "new - vytvorenie novej mapy\nload [local/server] - nacitanie mapy\nend - ukonci"
         << endl;

    bool run = true;

    getline(cin, command);
    cin.clear();

    if (command == "new") {
        int mapSize = 0;
        cout << "Zadajte velkost mapy: " << endl;
        cin >> mapSize;
        tileMap.MakeNew(mapSize);
    } else if (command.substr(0, 10) == "load local") {
        tileMap.LoadFromFile();
    } else if (command.substr(0, 11) == "load server") {
        tileMap.LoadFromServer();
    } else if (command == "end") {
        run = false;
    }

    while (run) {
        system("cls");
        tileMap.Print();

        cout << endl;
        cout <<
             "*ENTER* - krok simulacie\nfire [x] [y] - zapalenie biotopu\nsave [local/server] - ulozi mapu\nload [local/server] - nacita mapu\nend - ukonci"
             << endl;

        getline(cin, command);
        cin.clear();
        if (command == "end") {
            run = false;
        } else if (command.substr(0, 4) == "fire") {
            int x, y;
            istringstream iss(command.substr(5));
            if (iss >> x >> y) {
                if (tileMap.Fire(x - 1, y - 1)) {
                    cout << "Zapal na pozicii: [" << x << ", " << y << "]" << endl;
                } else {
                    cout << "Tento typ biotopu nemoze horiet" << endl;
                }
            } else {
                cout << "Neznamy prikaz" << endl;
            }
            getline(cin, command);
        } else if (command.substr(0, 10) == "save local") {
            tileMap.SaveToFile();
        } else if (command.substr(0, 11) == "save server") {
            tileMap.SaveToServer();
        } else if (command.substr(0, 10) == "load local") {
            tileMap.LoadFromFile();
        } else if (command.substr(0, 11) == "load server") {
            tileMap.LoadFromServer();
        } else {
            tileMap.Step();
        }
    }

    delete clientSocket;
    clientSocket = nullptr;

    return 0;
}