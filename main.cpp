#include "Client.cpp"
#include "Server.cpp"
#include "types.h"
#include <iostream>
using namespace std;

// Os clients devem mandar as mensagens automaticamente (seria legal se fosse aleatoriamente) - Ian
// Implementar delete, update, select, select_all - Duda

int main(int argc, char *argv[]) {

    PipeConnection serverPipe;
    PipeConnection clientPipe;
    pid_t pid;
    srand(time(NULL));

    if (pipe((int *)&serverPipe) == -1 || pipe((int *)&clientPipe) == -1) {
        perror("pipe");
        return 1;
    }

    pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid != 0) { // pFilho
        cout << "Server started" << endl;
        close(serverPipe.writeFd);
        Server server(serverPipe.readFd, clientPipe.writeFd);
        server.start();
        close(serverPipe.readFd);
    } else { // pPai
        close(clientPipe.writeFd);
        Client client(clientPipe.readFd, serverPipe.writeFd);
        if (argc == 2) {
            client.start(true, argv[1]);
        } else {
            client.start();
        }
    }

    // if (pid != 0) { // pFilho
    //     cout << "Server started" << endl;
    //     close(serverPipe.writeFd);
    //     Server server(serverPipe.readFd, serverPipe.writeFd);
    //     server.start();
    //     close(serverPipe.readFd);
    // } else { // pPai
    //     close(serverPipe.readFd);
    //     Client client(serverPipe.readFd, serverPipe.writeFd);
    //     client.start();
    //     close(serverPipe.writeFd);
    // }

    return 0;
}
