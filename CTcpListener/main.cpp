#include <iostream>
#include <string>

#include "CTcpListener.h"
#include "Qotd.h"

using namespace std;

void onMessageReceived(CTcpListener *listener, SOCKET sock, string message);

Qotd quotes("wisdom.txt");
int main()
{
    CTcpListener myServerListener = CTcpListener("192.168.1.6", 777, onMessageReceived);
    if (myServerListener.init())
    {
        myServerListener.runMultiThread();
    }
    return 0;
}

void onMessageReceived(CTcpListener *listener, SOCKET sock, string message)
{
    listener->sendMessage(sock, quotes.getRandomQuotes());
}