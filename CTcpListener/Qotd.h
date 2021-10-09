#ifndef SOCKETCLIENT_QOTD_H_
#define SOCKETCLIENT_QOTD_H_

#include <string>
#include <vector>
#include <fstream>

using namespace std;

class Qotd
{
public:
    Qotd(string filename);
    string getRandomQuotes();

private:
    vector<string> _quotes;
};
#endif //SOCKETCLIENT_QOTD_H_