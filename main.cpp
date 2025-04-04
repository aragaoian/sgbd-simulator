// #include "Client.cpp"
#include "Server.cpp"
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

    if (pid != 0) {
        cout << "Server started" << endl;
        close(fd[1]);
        Server server(fd[0]);
        server.start();
        close(fd[0]);
    } else {
        close(fd[0]);
        sendMessage(fd[1], "insert id=1 nome='Ian'");
        sendMessage(fd[1], "insert id=2 nome='Lucas'");
        sendMessage(fd[1], "insert id=3 nome='Duda'");
        close(fd[1]);
    }

    return 0;
}
