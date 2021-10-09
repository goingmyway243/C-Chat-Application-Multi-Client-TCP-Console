#include "Qotd.h"

Qotd::Qotd(string filename)
{
    ifstream file;
    file.open(filename);
    if (file.is_open())
    {
        string line;
        string quote = "";
        while (getline(file, line))
        {
            if (line != "%")
            {
                quote += line + "\n";
            }
            else
            {
                _quotes.push_back(quote);
                quote = "";
            }
        }
    }
}

string Qotd::getRandomQuotes()
{
    int randomNumber = rand() % _quotes.size();
    return _quotes[randomNumber];
}