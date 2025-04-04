#include "CommandHandler.cpp"
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <thread>
#include <unistd.h>
#include <vector>
using namespace std;

#define MAX_THREADS 16
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

typedef struct {
    char buffer[BUFFER_SIZE];
} Task;

class Server {
  public:
    Server(int fdRead) : fdRead(fdRead) {
        pthread_mutex_init(&logMutex, NULL);
        pthread_mutex_init(&queueMutex, NULL);
        pthread_cond_init(&newTaskCondition, NULL);

        for (int i = 0; i < MAX_THREADS; ++i) {
            cout << "Creating thread " << i << endl;
            threadPool[i] = thread(&Server::worker, this);
        }
    }

    ~Server() {
        pthread_mutex_lock(&queueMutex);
        isShuttingDown = true;
        pthread_cond_broadcast(&newTaskCondition); // wake up all waiting threads
        pthread_mutex_unlock(&queueMutex);

        for (int i = 0; i < MAX_THREADS; ++i) {
            cout << "Joining thread " << i << endl;
            threadPool[i].join();
        }
        pthread_mutex_destroy(&logMutex);
        pthread_mutex_destroy(&queueMutex);
        pthread_cond_destroy(&newTaskCondition);
    }

    void start() {

        while (1) {
            int len = 0;                                 // tamanho da mensagem
            ssize_t n = read(fdRead, &len, sizeof(int)); // lê o tamanho da próxima mensagem
            if (n < 0) {
                perror("Error reading message size");
                break;
            }
            if (n == 0) {
                cout << "Client disconnected" << endl;
                break;
            }

            if (len <= 0 || len >= BUFFER_SIZE) {
                cerr << "Invalid message length: " << len << endl;
                continue;
            }

            char buffer[BUFFER_SIZE];
            ssize_t totalRead = 0;
            while (totalRead < len) {
                // buffer aponta para o inicio da mensagem, estamos tamanho - lido bytes a partir de inicio + lido
                ssize_t bytesRead = read(fdRead, buffer + totalRead, len - totalRead);
                if (bytesRead <= 0) {
                    perror("Error reading message body");
                    break;
                }
                totalRead += bytesRead;
            }

            buffer[len] = '\0';

            pthread_mutex_lock(&queueMutex);
            Task task;
            strncpy(task.buffer, buffer, sizeof(task.buffer) - 1);
            task.buffer[sizeof(task.buffer) - 1] = '\0';
            queue.push_back(task);
            pthread_cond_signal(&newTaskCondition);
            pthread_mutex_unlock(&queueMutex);
        }
    }

  private:
    thread threadPool[MAX_THREADS];
    CommandHandler commandHandler;
    int fdRead;
    pthread_mutex_t logMutex;
    pthread_mutex_t queueMutex;
    pthread_cond_t newTaskCondition;
    vector<Task> queue;
    bool isShuttingDown = false;

    void *worker() {
        while (1) {
            pthread_mutex_lock(&queueMutex);

            while (queue.empty() && !isShuttingDown) {
                pthread_cond_wait(&newTaskCondition, &queueMutex);
            }

            if (isShuttingDown && queue.empty()) {
                pthread_mutex_unlock(&queueMutex);
                break;
            }

            // Pega a tarefa da fila
            Task task = queue.front();
            queue.erase(queue.begin());
            pthread_mutex_unlock(&queueMutex);

            string msg = "Processing task: " + string(task.buffer) + " on thread " + to_string(pthread_self());
            if (string(task.buffer) == "insert id=1 nome='Ian'") {
                sleep(4);
            }
            cout << msg << endl;

            handleClient(task.buffer);
        }

        return NULL;
    }

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
