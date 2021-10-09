#include <iostream>
#include <conio.h>
#include <string>
#include "CTcpClient.h"

using namespace std;

int main()
{
    CTcpClient client = CTcpClient("192.168.1.6", 777);
    if (client.init())
    {
        client.run();
    }
    system("pause");
    return 0;
}