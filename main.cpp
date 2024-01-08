
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
         "new - vytvorenie novej mapy\nload [nazov mapy] - nacitanie mapy\nend - ukonci"
         << endl;

    bool run = true;

    if (command == "new") {
        int mapSize = 10;
        cout << "Zadajte velkost mapy: " << endl;
        cin >> mapSize;
        tileMap.MakeNew(mapSize);
    } else if (command == "load") {
        tileMap.LoadFromFile("a.txt");
    } else if (command == "end") {
        run = false;
    }

    while (run) {
        system("cls");
        tileMap.Step();
        tileMap.Print();

        cout << endl;
        cout <<
             "*ENTER* - krok simulacie\nfire [x] [y] - zapalenie biotopu\nsave [nazov] - ulozi mapu\nload [nazov] - nacita mapu\nend - ukonci"
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
        } else if (command.substr(0, 4) == "save") {
            tileMap.SaveToServer(command.substr(5));
        } else if (command.substr(0, 4) == "load") {
            tileMap.LoadFromServer(command.substr(5));
        } else {
            cout << "Neznamy prikaz" << endl;
        }
    }

    delete clientSocket;
    clientSocket = nullptr;

    return 0;
}