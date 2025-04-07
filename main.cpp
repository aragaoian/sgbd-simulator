#include "Server.cpp"
#include "Client.cpp"
#include <iostream>
using namespace std;

// Os clients devem mandar as mensagens automaticamente (seria legal se fosse aleatoriamente) - Ian
// Implementar delete, update, select, select_all - Duda

struct PipeConnection {
    int readFd;
    int writeFd;
};

int main(int argc, char *argv[]) {
    int serverFd[2];
    int clientFd[2];
    // fd[0] -> leitura
    // fd[1] -> escrita
    pid_t pid;

    if (pipe(serverFd) == -1 || pipe(clientFd) == -1) {
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
        close(serverFd[1]);
        Server server(serverFd[0], clientFd[1]);
        server.start();
        close(serverFd[0]);
    } else { // pPai
        close(clientFd[1]);
        Client client(clientFd[0], serverFd[1], pid);
        client.start();
        close(clientFd[0]);
    }

    return 0;
}
