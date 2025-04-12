#pragma once
#include <string>
using namespace std;
#define BUFFER_SIZE 1500

struct PipeConnection {
    int readFd;
    int writeFd;
};

typedef struct {
    char buffer[BUFFER_SIZE];
} Task;

typedef struct {
    int id;
    string nome;
} Registro;

typedef struct {
    string name;
    string value;
} Arg;
