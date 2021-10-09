#ifndef SOCKETCLIENT_CTCPCLIENT_H_
#define SOCKETCLIENT_CTCPCLIENT_H_

#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <thread>
#include <conio.h>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

#define DEFAULT_BUFLEN 4096

class CTcpClient
{
public:
    CTcpClient(string ipAddress, int port);
    ~CTcpClient();

    bool init();
    void cleanup();
    void sendMessage(string message);
    void receiveMessage();
    void run();

private:
    SOCKET _connectSocket;
    string _ipAddress;
    int _port;
    bool _isConnected;
    string _name;
};
#endif //SOCKETCLIENT_CTCPCLIENT_H_