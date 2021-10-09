#include <iostream>
#include <WS2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib");

using namespace std;

#define DEFAULT_BUFLEN 4096
#define PORT 777
#define IPADDRESS "192.168.1.2"

int main()
{
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "Startup failed" << endl;
        return -1;
    }

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
    {
        cout << "Create socket failed" << endl;
        WSACleanup();
        return -1;
    }

    sockaddr_in hints;
    hints.sin_family = AF_INET;
    hints.sin_port = htons(PORT);
    hints.sin_addr.S_un.S_addr = inet_addr(IPADDRESS);

    if (bind(listenSocket, (const sockaddr *)&hints, sizeof(hints)) == SOCKET_ERROR)
    {
        cout << "Bind failed" << endl;
        closesocket(listenSocket);
        WSACleanup();
        return -1;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        cout << "Listen failed" << endl;
        closesocket(listenSocket);
        WSACleanup();
        return -1;
    }

    sockaddr_in clientInfo;
    int infoSize = sizeof(clientInfo);

    SOCKET clientSocket = accept(listenSocket, (sockaddr *)&clientInfo, &infoSize);

    if (clientSocket != INVALID_SOCKET)
    {
        closesocket(listenSocket);
        cout << "Connected: " << inet_ntoa(clientInfo.sin_addr) << endl;

        char buff[DEFAULT_BUFLEN];
        while (true)
        {
            ZeroMemory(buff, DEFAULT_BUFLEN);
            int byteRecv = recv(clientSocket, buff, DEFAULT_BUFLEN, 0);
            if (byteRecv < 0)
            {
                cout << "Receive failed" << endl;
                break;
            }
            else if (byteRecv == 0)
            {
                cout << "Client disconnect" << endl;
                break;
            }

            cout << buff << endl;

            strcat(buff, " replied");
            send(clientSocket, buff, DEFAULT_BUFLEN, 0);
        }
        closesocket(clientSocket);
    }

    WSACleanup();
    system("pause");
    return 0;
}