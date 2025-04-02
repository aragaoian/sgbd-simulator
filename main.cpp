#include <iostream>
#include "Server.cpp"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Usage: port" << endl;
        return 1;
    }

    int port = atoi(argv[1]);
    Server server(port);
    server.start();

    return 0;
}
