#include <iostream>
#include <string.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include "Names.h"
using namespace std;

#define BUFFER_SIZE 1500
#define COMMAND_MAX 8
#define COLUMN_MAX 2
#define VALUE_MAX 100


class Client {
  public:
    Client(int fdRead, int fdWrite, int clientId) : fdRead(fdRead), 
                                                    fdWrite(fdWrite), 
                                                    clientId(clientId) {}

    void start() {
        // clientResponse(fdRead);
        sendMessage(fdWrite, commandList[commandId]);
    }

    static string buildString(const vector <string> &prefix, bool isNumber, bool isBoth) {

        /*
        Parameters:
            prefix -> vetor para guardar o(s) prefixo(s)
            isNumber -> true (int) | false (string)
            isBoth -> true (tanto int como string) | false (nada)
        */

        std::stringstream ss;
        string id = to_string(rand() % VALUE_MAX);
        string name = namesArray[rand() % VALUE_MAX];
  
        if(!isBoth){
            ss << prefix[0];
            if (isNumber) {
                ss << id;
            } else {
                ss << "'" << name << "'";
            }
        }else{
            ss << prefix[0] << id << prefix[1] << "'" << name << "'";
        }
    
        return ss.str();
    }

  private:
    int fdRead;
    int fdWrite;
    int clientId;
    int commandId = rand() % (COMMAND_MAX-6);
    string commandList[COMMAND_MAX] = {
        "SELECT * FROM Table",
        []() { return buildString({"insert id=", " nome="}, false, true); }(),
        // []() { return buildString({"select nome where id="}, true, false); }(),
        // []() { return buildString({"select id where nome="}, false, false); }(),
        // []() { return buildString({"delete where nome="}, false, false); }(),
        // []() { return buildString({"delete where id="}, true, false); }(),
        // []() { return buildString({"update nome="}, false, false); }(),
        // []() { return buildString({"update id="}, true, false); }()
    };

    void sendMessage(int fd, const string &message) {
        int len = message.size();
        write(fd, &len, sizeof(int));    // envia o tamanho
        write(fd, message.c_str(), len); // envia a mensagem
    }

    void clientResponse(int fd){
        return;
    }
};