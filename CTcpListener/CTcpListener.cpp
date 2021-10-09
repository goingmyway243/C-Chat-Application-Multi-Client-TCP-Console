#include "CTcpListener.h"
#include <thread>

CTcpListener::CTcpListener(string ipAddress, int port, messageReceivedCallback handler)
{
    _ipAddress = ipAddress;
    _port = port;
    _messageHandler = handler;
}

CTcpListener::~CTcpListener()
{
    cleanup();
}

void CTcpListener::sendMessage(SOCKET sock, string message)
{
    send(sock, message.c_str(), message.length(), 0);
}

bool CTcpListener::init()
{
    WSAData wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
}

SOCKET CTcpListener::createSocket()
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock != INVALID_SOCKET)
    {
        sockaddr_in hints;
        hints.sin_family = AF_INET;
        hints.sin_port = htons(_port);
        hints.sin_addr.S_un.S_addr = inet_addr(_ipAddress.c_str());

        if (bind(sock, (const sockaddr *)&hints, sizeof(hints)) != SOCKET_ERROR)
        {
            if (listen(sock, SOMAXCONN) != SOCKET_ERROR)
            {
                return sock;
            }
        }
    }
    return INVALID_SOCKET;
}

SOCKET CTcpListener::waitForConnection(SOCKET listenSocket)
{
    sockaddr_in clientInfo;
    int infoSize = sizeof(clientInfo);

    SOCKET sock = accept(listenSocket, (sockaddr *)&clientInfo, &infoSize);
    cout << "Connected: " << inet_ntoa(clientInfo.sin_addr) << " at Socket #" << sock << endl;
    return sock;
}

void CTcpListener::run()
{
    char buff[DEFAULT_BUFLEN];
    while (true)
    {
        SOCKET listenSocket = createSocket();
        if (listenSocket == INVALID_SOCKET)
        {
            break;
        }

        SOCKET clientSocket = waitForConnection(listenSocket);
        if (clientSocket != INVALID_SOCKET)
        {
            closesocket(listenSocket);
            int byteReceived = 0;

            while (true)
            {
                ZeroMemory(buff, DEFAULT_BUFLEN);

                byteReceived = recv(clientSocket, buff, DEFAULT_BUFLEN, 0);
                if (byteReceived < 0)
                {
                    cout << "Receive failed" << endl;
                    break;
                }
                else if (byteReceived == 0)
                {
                    cout << "Client disconnect" << endl;
                    break;
                }

                if (_messageHandler)
                {
                    strcat(buff, " replied");
                    _messageHandler(this, clientSocket, string(buff, 0, sizeof(buff)));
                }
            }
            closesocket(clientSocket);
        }
    }
}

void CTcpListener::runMultiThread()
{
    SOCKET listenSocket = createSocket();
    if (listenSocket == INVALID_SOCKET)
    {
        return;
    }

    FD_ZERO(&_socketList);

    auto f = [this, &listenSocket](SOCKET sock)
    {
        char buff[DEFAULT_BUFLEN];
        int byteReceived = 0;

        recv(sock, buff, DEFAULT_BUFLEN, 0);
        string name = buff;

        for (SOCKET outSock : _socketList.fd_array)
        {
            ostringstream os;
            os << "-----> " << name << " (#" << sock << ")"
               << " has join to chat room ! <-----";
            string notify = os.str();
            sendMessage(outSock, notify);
        }

        while (true)
        {
            ZeroMemory(buff, DEFAULT_BUFLEN);

            byteReceived = recv(sock, buff, DEFAULT_BUFLEN, 0);
            if (byteReceived < 0)
            {
                cout << "Receive failed" << endl;
                break;
            }
            else if (byteReceived == 0)
            {
                cout << "Client disconnect" << endl;
                break;
            }

            string check = buff;
            if (check == "exit")
            {
                ostringstream os;
                os << "Bye " << name << " (#" << sock << ") !" << buff;
                string notify = os.str();
                cout << notify << endl;
                sendMessage(sock, "");

                for (int i = 0; i < _socketList.fd_count; i++)
                {
                    SOCKET outSock = _socketList.fd_array[i];
                    if (outSock != sock)
                    {
                        ostringstream os;
                        os << "<----- " << name << " (#" << outSock << ") has left chat room ! ----->";
                        string message = os.str();
                        sendMessage(outSock, message);
                    }
                }
                break;
            }
            for (int i = 0; i < _socketList.fd_count; i++)
            {
                SOCKET outSock = _socketList.fd_array[i];
                if (outSock != sock)
                {
                    ostringstream os;
                    os << name << " (#" << outSock << "): " << buff;
                    string message = os.str();
                    sendMessage(outSock, message);
                }
            }
        }
        FD_CLR(sock, &_socketList);
        closesocket(sock);
    };

    while (true)
    {
        SOCKET clientSocket = waitForConnection(listenSocket);
        if (clientSocket != INVALID_SOCKET)
        {
            FD_SET(clientSocket, &_socketList);
            thread th(f, clientSocket);
            th.detach();
        }
    }
}

void CTcpListener::cleanup()
{
    WSACleanup();
}
