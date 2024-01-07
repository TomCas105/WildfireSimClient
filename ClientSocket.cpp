#include "ClientSocket.h"
#include <stdexcept>
#include <ws2tcpip.h>
#include <winsock2.h>

#define SOCKET_TERMINATE_CHAR '\0'

const char* ClientSocket::endMessage = ":end";  // upravit podla potreby

ClientSocket* ClientSocket::createConnection(std::string hostName, short port) {
    WSADATA wsaData;
    struct addrinfo *result = NULL;
    struct addrinfo hints;
    int iResult;

    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);  // STARTUP
    if (iResult != 0) {
        throw std::runtime_error("WSAStartup failed with error: " + std::to_string(iResult) + "\n");
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(hostName.c_str(), std::to_string(port).c_str(), &hints, &result);
    if (iResult != 0) {
        WSACleanup();   // CLEANUP
        throw std::runtime_error("getaddrinfo failed with error: " + std::to_string(iResult) + "\n");
    }

    // Create a SOCKET for connecting to server
    SOCKET connectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (connectSocket == INVALID_SOCKET) {
        WSACleanup();
        throw std::runtime_error("socket failed with error: " + std::to_string(WSAGetLastError()) + "\n");
    }

    // Connect to server
    iResult = connect(connectSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        closesocket(connectSocket);
        connectSocket = INVALID_SOCKET;
    }

    freeaddrinfo(result);

    if (connectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        throw std::runtime_error("Unable to connect to server.\n");
    }

    return new ClientSocket(connectSocket);
}

ClientSocket::ClientSocket(SOCKET socket) :
        connectSocket(socket) {

}

ClientSocket::~ClientSocket() {
    if (this->connectSocket != INVALID_SOCKET) {
        closesocket(this->connectSocket);
        this->connectSocket = INVALID_SOCKET;
    }
    WSACleanup();
}

void ClientSocket::sendData(const std::string &data) {
    size_t data_length = data.length();
    char* buffer = (char*)calloc(data_length + 1, sizeof(char));
    memcpy(buffer, data.c_str(), data_length);
    buffer[data_length] = SOCKET_TERMINATE_CHAR;

    int iResult = send(connectSocket, buffer, data_length + 1, 0 );
    if (iResult == SOCKET_ERROR) {
        throw std::runtime_error("send failed with error: " + std::to_string(WSAGetLastError()) + "\n");
    }
    free(buffer);
    buffer = NULL;
}

void ClientSocket::sendEndMessage() {
    this->sendData(this->endMessage);
}

#undef SOCKET_TERMINATE_CHAR