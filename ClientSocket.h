#ifndef SOCKETS_CLIENT_MY_SOCKET_H
#define SOCKETS_CLIENT_MY_SOCKET_H

#include <winsock2.h>
#include <string>

class ClientSocket {
public:
    static ClientSocket* createConnection(std::string hostName, short port);
    ~ClientSocket();
    void sendData(const std::string& data);
    void sendEndMessage();
protected:
    ClientSocket(SOCKET socket);
private:
    static const char * endMessage;
    SOCKET connectSocket;
};

#endif //SOCKETS_CLIENT_MY_SOCKET_H
