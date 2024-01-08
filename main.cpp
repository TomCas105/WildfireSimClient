#include <iostream>
#include <fstream>
#include <sstream>
//#include "TileMap.h"
//#include "ClientSocket.h"

using namespace std;

int main() {
    srand(time(nullptr));

    int mapSize = 10;

    cout << "Zadajte velkost mapy: " << endl;
    cin >> mapSize;
    cin.ignore();
    cin.clear();

    //Socket init
    //ClientSocket* clientSocket = ClientSocket::createConnection("frios2.fri.uniza.sk", 11889);

    //Map init
    //TileMap tileMap;

    //tileMap.MakeNew(mapSize);

    bool run = true;

    while (run) {
        system("cls");
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
                if (/*tileMap.Fire(x, y)*/true) {
                    cout << "Zapal na pozicii: [" << x << ", " << y << "]" << endl;
                } else {
                    cout << "Tento typ biotopu nemoze horiet" << endl;
                }
            } else {
                cout << "Neznamy prikaz" << endl;
            }
        } else if (command == "save") {
            // TODO ulozenie mapy
            ofstream file("data.txt");
            if (file.is_open()) {
                file << mapSize << "\n";
                file.close();
                cout << "Mapa je ulozena lokalne" << endl;
            } else {
                cerr << "Nepodarilo sa otvorit subor pre lokalny zapis." << endl;
            }
        } else if (command == "load") {
            // TODO nacitanie mapy
            ifstream file("data.txt");
            if (file.is_open()) {
                while (!file.eof()) {
                    file >> mapSize;
                }
                file.close();
            } else {
                cerr << "Nepodarilo sa otvorit subor pre lokalne citanie" << endl;
            }
        } else {
            cout << "Neznamy prikaz" << endl;
        }
        cout << endl << endl << endl;

        //tileMap.Step();
        //tileMap.Print();
    }

    //delete clientSocket;
    //clientSocket = nullptr;

    return 0;
}