#include "Names.h"
#include "Utils.cpp"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

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

    void start(bool userInput = false, string userMessage = "") {
        if (!userInput) {
            for (int i = 0; i < reqMax; i++) {

                sendMessage(commandList[rand() % (COMMAND_MAX)]());
            }
            while (resCount != reqMax) {
            }
        } else {
            sendMessage(userMessage);
            while (resCount != 1) {
            }
        }
    }

    static string buildString(const vector<string> &prefix, bool isNumber, bool isBoth) {

        /*
        Parameters:
            prefix -> vetor para guardar o(s) prefixo(s)
            isNumber -> true (int) | false (string)
            isBoth -> true (tanto int como string) | false (nada)
        */

        std::stringstream ss;
        string id = to_string(rand() % VALUE_MAX);
        string name = namesArray[rand() % VALUE_MAX];

        if (!isBoth) {
            ss << prefix[0];
            if (isNumber) {
                ss << id;
            } else {
                ss << "'" << name << "'";
            }
        } else {
            ss << prefix[0] << id << prefix[1] << "'" << name << "'";
        }

        return ss.str();
    }

  private:
    thread listener_thread;
    int fdRead;
    int fdWrite;
    int reqMax = rand() % REQ_MAX + 1;
    int resCount = 0;
    using CommandFunction = string (*)();
    CommandFunction commandList[COMMAND_MAX] = {[]() { return string("select nome id"); },
                                                []() { return buildString({"insert id=", " nome="}, false, true); },
                                                []() { return buildString({"select nome where id="}, true, false); },
                                                []() { return buildString({"select id where nome="}, false, false); },
                                                []() { return buildString({"delete where nome="}, false, false); },
                                                []() { return buildString({"delete where id="}, true, false); },
                                                []() { return buildString({"update nome="}, false, false); }};

    void *serverResponse() {

        while (1) {

            pair<string, int> result = readPipeMessage(fdRead, "Client");
            if (result.second == 1) {
                break;
            }
            if (result.second == 2) {
                continue;
            }

            cout << "Server Response: " << result.first;

            resCount++;
        }

        return NULL;
    }

    void sendMessage(const string &message) {
        int len = message.size();
        write(fdWrite, &len, sizeof(int));    // envia o tamanho
        write(fdWrite, message.c_str(), len); // envia a mensagem
    }
};