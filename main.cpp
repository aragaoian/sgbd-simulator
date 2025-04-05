#include "Server.cpp"
#include "Client.cpp"
#include <iostream>

// Os clients devem mandar as mensagens automaticamente (seria legal se fosse aleatoriamente) - Ian
// Implementar delete, update, select, select_all - Duda

using namespace std;

void sendMessage(int fd, const string &message) {
    int len = message.size();
    write(fd, &len, sizeof(int));    // envia o tamanho
    write(fd, message.c_str(), len); // envia a mensagem
}

int main(int argc, char *argv[]) {
    int fd[2];
    // fd[0] -> leitura
    // fd[1] -> escrita
    pid_t pid;

    if (pipe(fd) == -1) {
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
        close(fd[1]);
        Server server(fd[0]);
        server.start();
        close(fd[0]);
    } else { // pPai
        close(fd[0]);
        Client client(fd[0], fd[1], pid);
        client.start();
        close(fd[1]);
    }

    return 0;
}
