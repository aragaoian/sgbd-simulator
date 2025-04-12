#include <iostream>
#include "types.h"
#include <string.h>
#include <unistd.h>
using namespace std;

void *readPipeMessage(int fdRead, string type){
    while (1) {
        int len = 0;                                 // tamanho da mensagem
        ssize_t n = read(fdRead, &len, sizeof(int)); // lê o tamanho da próxima mensagem

        if (n < 0) {
            perror((type + ": Error reading message size\n").c_str());
            break;
        }
        if (n == 0) {
            cout << type + ": Client disconnected\n";
            break;
        }

        if (len <= 0 || len >= BUFFER_SIZE) {
            cerr << type + ": Invalid message length: " + to_string(len)  + "\n";
            continue;
        }

        char buffer[BUFFER_SIZE];
        ssize_t totalRead = 0;
        while (totalRead < len) {
            // buffer + totalRead -> inicio + lugar que estamos
            // len - totalRead -> quanto falta para o fim
            ssize_t bytesRead = read(fdRead, buffer + totalRead, len - totalRead);
            if (bytesRead <= 0) {
                perror((type + ": Error reading message body").c_str());
                break;
            }
            totalRead += bytesRead;
        }

        buffer[len] = '\0';

        if(type == "Server"){
            cout << type + ": " + buffer + "\n";
        }

    }
}

void writePipeMessage(int fdWrite, const string &message) {
    int len = message.size();
    write(fdWrite, &len, sizeof(int));    // envia o tamanho
    write(fdWrite, message.c_str(), len); // envia a mensagem
}