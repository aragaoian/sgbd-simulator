#include "CommandHandler.cpp"
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <thread>
#include <unistd.h>
#include <vector>
using namespace std;

#define BUFFER_SIZE 1500
#define MAX_ARGS 10

vector<Arg> tokenize(const string &buf) {
    vector<Arg> args;
    istringstream ss(buf);
    char ch;
    bool inQuotes = false;
    string token;
    string key;
    bool readingValue = false;

    while (ss >> noskipws >> ch) {
        if (ch == '\'') {
            inQuotes = !inQuotes;
            continue; // Pula o caractere de aspas
        }

        if (ch == '=' && !inQuotes && !readingValue) {
            key = token;
            token.clear();
            readingValue = true;
        } else if (isspace(ch) && !inQuotes) {
            if (!token.empty()) {
                if (readingValue) {
                    args.push_back({key, token});
                    readingValue = false;
                    key.clear();
                } else {
                    args.push_back({token, ""});
                }
                token.clear();
            }
        } else {
            token += ch;
        }
    }

    if (!token.empty()) {
        if (readingValue) {
            args.push_back({key, token});
        } else {
            args.push_back({token, ""});
        }
    }

    return args;
}

class Server {
  public:
    Server(int fdRead) : fdRead(fdRead) {}

    void start() {
        while (1) {
            char buffer[128];
            ssize_t n = read(fdRead, buffer, sizeof(buffer) - 1);
            if (n > 0) {
                buffer[n] = '\0'; // Null-terminate the string
                thread(&Server::handleClient, this, buffer).detach();
            }
        }
    }

  private:
    CommandHandler commandHandler;
    int fdRead;
    pthread_mutex_t logMutex;

    void *handleClient(char *buffer) {
        string command(buffer);
        vector<Arg> args = tokenize(buffer);

        if (args.empty()) return NULL;

        string response = commandHandler.executeCommand(args[0].name, args.size(), args);

        time_t now = time(NULL);                                            // get current time
        tm *ltm = localtime(&now);                                          // convert to local time
        char timeBuffer[80];                                                // buffer for time string
        strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", ltm); // format time
        string timeString(timeBuffer);                                      // convert to string

        // put response in log.txt
        pthread_mutex_lock(&logMutex);
        ofstream logFile("log.txt", ios::app);
        logFile << timeString << " - " << command << ": " << response;
        logFile.close();
        pthread_mutex_unlock(&logMutex);
        return NULL;
    }
};
