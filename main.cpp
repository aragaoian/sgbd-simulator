// #include "Client.cpp"
#include "Server.cpp"
#include <iostream>

// Criar diversos clients - Ian
// Os clients devem mandar as mensagens automaticamente (seria legal se fosse aleatoriamente) - Ian
// Implementar delete, update, select, select_all - Lucas

using namespace std;

int main(int argc, char *argv[]) {
    int fd[2];
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

    if (pid != 0) {
        cout << "Server started" << endl;
        close(fd[1]);
        Server server(fd[0]);
        server.start();
        close(fd[0]);
    } else {
        close(fd[0]); // Fecha a leitura
        const char *msg = "insert id=2 nome='Lucas'";
        write(fd[1], msg, strlen(msg));
        close(fd[1]);
    }

    return 0;
}
