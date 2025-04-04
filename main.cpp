#include "Client.cpp"
#include "Server.cpp"
#include <iostream>

// Servidor deve ser com PIPE - LUCAS e IAN
// Salvar as respostas em um arquivo - Lucas
// Criar diversos clients - Ian
// Comunicar os clients de que o servidor está aberto - Ian
// Os clients devem mandar as mensagens automaticamente (seria legal se fosse aleatoriamente) - Ian
// Implementar delete, update, select, select_all - Lucas
// Insert nao deve aceitar id duplicado - Lucas

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Usage: port hostname" << endl;
        return 1;
    }

    pid_t pid;
    const char *hostName = argv[2];
    int port = atoi(argv[1]);

    pid = fork();
    if (pid == 0) {
        cout << "Port: " << port << endl;
        Server server(port);
        server.start();
    } else {
        sleep(3);
        Client client(port, hostName);
        client.start();
    }

    return 0;
}
