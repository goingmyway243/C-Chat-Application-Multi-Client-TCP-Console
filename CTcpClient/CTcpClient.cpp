#include "CTcpClient.h"

CTcpClient::CTcpClient(string ipAddress, int port)
{
    _ipAddress = ipAddress;
    _port = port;
    _isConnected = false;
    _name = "";
}

CTcpClient::~CTcpClient()
{
    cleanup();
}

bool CTcpClient::init()
{
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "Startup failed" << endl;
        return false;
    }

    _connectSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_connectSocket == INVALID_SOCKET)
    {
        cout << "Create socket failed" << endl;
        WSACleanup();
        return false;
    }

    sockaddr_in hints;
    hints.sin_family = AF_INET;
    hints.sin_port = htons(_port);
    hints.sin_addr.S_un.S_addr = inet_addr(_ipAddress.c_str());

    cout << "Please enter your name: ";
    cin >> _name;
    getchar();

    if (connect(_connectSocket, (const sockaddr *)&hints, sizeof(hints)) == SOCKET_ERROR)
    {
        cout << "Connect to server failed" << endl;
        closesocket(_connectSocket);
        WSACleanup();
        return false;
    }

    cout << "~ Connect success to " << inet_ntoa(hints.sin_addr) << " ~\n"
         << endl;
    sendMessage(_name);
    _isConnected = true;
    return true;
}

void CTcpClient::sendMessage(string message)
{
    send(_connectSocket, message.c_str(), message.length(), 0);
}

void CTcpClient::receiveMessage()
{
    char buff[DEFAULT_BUFLEN];
    ZeroMemory(buff, DEFAULT_BUFLEN);
    recv(_connectSocket, buff, DEFAULT_BUFLEN, 0);
    if (buff == "exit")
        _isConnected = false;

    cout << buff << endl;
}

void CTcpClient::run()
{
    auto writeF = [this]()
    {
        string message;
        while (_isConnected)
        {
            if (_kbhit())
            {
                cout << "> ";
                fflush(stdout);
                getline(cin, message);
                sendMessage(message);
                if (message == "exit")
                {
                    _isConnected = false;
                    // cleanup();
                    break;
                }
            }
        }
    };

    auto readF = [this]()
    {
        while (_isConnected)
        {
            receiveMessage();
        }
        cleanup();
    };

    thread readThread(readF);
    thread writeThread(writeF);

    readThread.join();
    writeThread.join();
}

void CTcpClient::cleanup()
{
    if (_connectSocket)
        closesocket(_connectSocket);

    WSACleanup();
}