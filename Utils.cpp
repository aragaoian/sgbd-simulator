#pragma once
#include "types.h"
#include <iostream>
#include <string.h>
#include <unistd.h>
using namespace std;

pair<const char *, int> readPipeMessage(int fdRead, string type) {

    int len = 0;                                 // tamanho da mensagem
    ssize_t n = read(fdRead, &len, sizeof(int)); // lê o tamanho da próxima mensagem

    if (n < 0) {
        perror((type + ": Error reading message size\n").c_str());
        return {"", 1};
    }
    if (n == 0) {
        cout << type + ": " + (type == "Server" ? "Client" : "Server") + " disconnected\n";
        return {"", 1};
    }

    if (len <= 0 || len >= BUFFER_SIZE) {
        cerr << type + ": Invalid message length: " + to_string(len) + "\n";
        return {"", 2};
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

    return {buffer, 0};
}

void writePipeMessage(int fdWrite, const string &message) {
    int len = message.size();
    write(fdWrite, &len, sizeof(int));    // envia o tamanho
    write(fdWrite, message.c_str(), len); // envia a mensagem
}