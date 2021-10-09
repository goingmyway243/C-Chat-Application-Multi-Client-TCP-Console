#include <iostream>
#include <string>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define PORT 777
#define IPADDRESS "192.168.1.2"
#define BUFLEN 4096

int main()
{
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "Startup failed" << endl;
        return -1;
    }

    // hostent *he;
    // if ((he = gethostbyname("www.google.com")) == NULL)
    // {
    //     //gethostbyname failed
    //     printf("gethostbyname failed : %d", WSAGetLastError());
    //     return 1;
    // }

    // //Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
    // in_addr **addr_list = (struct in_addr **)he->h_addr_list;
    // char *ip;
    // for (int i = 0; addr_list[i] != NULL; i++)
    // {
    //     //Return the first one;
    //     strcpy(ip, inet_ntoa(*addr_list[i]));
    // }

    // printf("%s resolved to : %s\n", "www.google.com", ip);

    SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (connectSocket == INVALID_SOCKET)
    {
        cout << "Create socket failed" << endl;
        WSACleanup();
        return -1;
    }

    sockaddr_in hints;
    hints.sin_family = AF_INET;
    hints.sin_port = htons(PORT);
    hints.sin_addr.S_un.S_addr = inet_addr(IPADDRESS);

    if (connect(connectSocket, (const sockaddr *)&hints, sizeof(hints)) == SOCKET_ERROR)
    {
        cout << "Connect failed" << endl;
        closesocket(connectSocket);
        WSACleanup();
        return -1;
    }

    char buff[BUFLEN];
    string message;
    while (true)
    {
        ZeroMemory(buff, BUFLEN);

        cout << ">";
        getline(cin, message);
        if (message == "exit")
            break;

        send(connectSocket, message.c_str(), message.length(), 0);

        int byteRecv = recv(connectSocket, buff, BUFLEN, 0);
        if (byteRecv <= 0)
        {
            cout << "Receive failed" << endl;
            break;
        }

        cout << buff << endl;
    }
    closesocket(connectSocket);
    WSACleanup();
    return 0;
}