#ifndef SOCKETCLIENT_CTCPLISTENER_H_
#define SOCKETCLIENT_CTCPLISTENER_H_

#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <thread>
#include <sstream>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

#define DEFAULT_BUFLEN 4096

class CTcpListener;

typedef void (*messageReceivedCallback)(CTcpListener *listener, SOCKET sock, string message);

class CTcpListener
{
public:
    CTcpListener(string ipAddress, int port, messageReceivedCallback handler);
    ~CTcpListener();

    void sendMessage(SOCKET sock, string message);
    bool init();

    void run();
    void runMultiThread();
    void cleanup();

private:
    SOCKET createSocket();
    SOCKET waitForConnection(SOCKET listenSocket);

private:
    string _ipAddress;
    int _port;
    messageReceivedCallback _messageHandler;
    fd_set _socketList;
};
#endif //SOCKETCLIENT_CTCPLISTENER_H_