#include <iostream>
#include <string.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include "Names.h"
using namespace std;

#define BUFFER_SIZE 1500
#define VALUE_MAX 100
#define REQ_MAX 16
#define COMMAND_MAX 8
#define COLUMN_MAX 2


class Client {
  public:
    // fdRead -> Client
    // fdWrite -> Server
    Client(int fdRead, int fdWrite) : fdRead(fdRead), fdWrite(fdWrite) {
        cout << "Client: Listener thread created" << endl;
        listener_thread = thread(&Client::serverResponse, this);
    }

    ~Client() {
        listener_thread.detach();
        close(fdRead);
    }

    void start(bool userInput=false, string userMessage="") {
        for(int i = 0; i < reqMax; i++){
            if(!userInput){
                sendMessage(commandList[commandId]());
            }else{
                sendMessage(userMessage);
            }
        }
        while(resCount != 0){}
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
    thread listener_thread;
    int fdRead;
    int fdWrite;
    int reqMax = rand() % REQ_MAX + 1;
    int resCount = reqMax;
    int commandId = rand() % (COMMAND_MAX);
    int commandId = 0;
    using CommandFunction = string(*)();
    CommandFunction commandList[COMMAND_MAX] = {
        []() { return string("select nome id"); },
        []() { return buildString({"insert id=", " nome="}, false, true); },
        []() { return buildString({"select nome where id="}, true, false); },
        []() { return buildString({"select id where nome="}, false, false); },
        []() { return buildString({"delete where nome="}, false, false); },
        []() { return buildString({"delete where id="}, true, false); },
        []() { return buildString({"update nome="}, false, false); },
        []() { return buildString({"update id="}, true, false); }
    };
    
    void *serverResponse(){
        
        while(1) {

            
            int len = 0;
            ssize_t n = read(fdRead, &len, sizeof(int)); // Lê o tamanho da mensagem
            if (n <= 0) { // Verifica se houve erro ou se a conexão foi fechada
                if (n == 0) {
                    cout << "Client: Connection closed by server" << endl;
                } else {
                    perror("Client: Error reading message size from server");
                }
                break;
            }
                        
            if (n != sizeof(int)) { // Checagem para ver se excedeu o tamanho da mensagem
                cout << "Client: Incomplete read of message size" << endl;
                break;
            }
            
            char buffer[BUFFER_SIZE];
            if (!buffer) {
                perror("Client: Memory allocation failed");
                break;
            }
            
            ssize_t totalRead = 0;
            while (totalRead < len) { // leitura do buffer
                // buffer + totalRead -> inicio + lugar que estamos
                // len - totalRead -> quanto falta para o fim
                ssize_t bytesRead = read(fdRead, buffer + totalRead, len - totalRead);
                if (bytesRead <= 0) {
                    perror("Client: Error reading message from Server");
                    return NULL;
                }
                totalRead += bytesRead;
            }
            
            buffer[len] = '\0';
            cout << "Server: " << buffer;

            resCount--;
        }
        
        return NULL;

    }

    void sendMessage(const string &message) {
        int len = message.size();
        write(fdWrite, &len, sizeof(int));    // envia o tamanho
        write(fdWrite, message.c_str(), len); // envia a mensagem
    }

};