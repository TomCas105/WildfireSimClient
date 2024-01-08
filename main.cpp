
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

    getline(cin, command);
    cin.clear();

    if (command == "new") {
        int mapSize = 0;
        cout << "Zadajte velkost mapy: " << endl;
        cin >> mapSize;
        tileMap.MakeNew(mapSize);
    } else if (command.substr(0, 4) == "load") {
        bool result = false;
        if (command.substr(6, 5) == "local") {
            tileMap.LoadFromFile(command.substr(12));
            result = true;
        } else if (command.substr(6, 6) == "server"){
            tileMap.LoadFromServer(command.substr(13));
            result = true;
        }
        if (result) {
            cout << "Mapa nacitana." << endl;
            getline(cin, command);
        }
    } else if (command == "end") {
        run = false;
    }

    while (run) {
        system("cls");
        tileMap.Step();
        tileMap.Print();

        cout << endl;
        cout <<
             "*ENTER* - krok simulacie\nfire [x] [y] - zapalenie biotopu\nsave [local/server] [nazov] - ulozi mapu\nload [local/server] [nazov] - nacita mapu\nend - ukonci"
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
            bool result = false;
            if (command.substr(6, 5) == "local") {
                tileMap.SaveToFile(command.substr(12));
                result = true;
            } else if (command.substr(6, 6) == "server"){
                tileMap.SaveToServer(command.substr(13));
                result = true;
            }
            if (result) {
                cout << "Mapa ulozena." << endl;
                getline(cin, command);
            }
        } else if (command.substr(0, 4) == "load") {
            bool result = false;
            if (command.substr(6, 5) == "local") {
                tileMap.LoadFromFile(command.substr(12));
                result = true;
            } else if (command.substr(6, 6) == "server"){
                tileMap.LoadFromServer(command.substr(13));
                result = true;
            }
            if (result) {
                cout << "Mapa nacitana." << endl;
                getline(cin, command);
            }
        } else {
            cout << "Neznamy prikaz" << endl;
        }
    }

    delete clientSocket;
    clientSocket = nullptr;

    return 0;
}