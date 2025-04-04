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
    Server(int port) : port(port) {}

    void start() {
        setupSocket();
        listenForClients();
    }

  private:
    int port;
    int serverSd;
    sockaddr_in servAddr;
    CommandHandler commandHandler;

    void setupSocket() {
        serverSd = socket(AF_INET, SOCK_STREAM, 0); // socket ID
        if (serverSd < 0) {
            cerr << "Error establishing the server socket" << endl;
            exit(EXIT_FAILURE);
        }

        bzero((char *)&servAddr, sizeof(servAddr));   // initialize servAddr
        servAddr.sin_family = AF_INET;                // IPv4
        servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Accept multiple address'
        servAddr.sin_port = htons(port);

        if (bind(serverSd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
            cerr << "Error binding socket to local address" << endl;
            exit(EXIT_FAILURE);
        }
    }

    void listenForClients() {
        unsigned int num_threads = thread::hardware_concurrency();
        listen(serverSd, num_threads);
        cout << "Waiting for clients..." << endl;
        // receive a request from client using accept
        // we need a new address to connect with the client

        socklen_t addrlen = sizeof(servAddr);
        while (1) {
            int new_socket = accept(serverSd, (struct sockaddr *)&servAddr, &addrlen);
            if (new_socket < 0) {
                cerr << "Error accepting request from client!" << endl;
                exit(1);
            }
            cout << "Client connected!" << endl;
            thread(&Server::handleClient, this, new_socket).detach();
        }
    }

    void *handleClient(int sock) {

        char buffer[BUFFER_SIZE];

        while (1) {
            memset(buffer, 0, BUFFER_SIZE); // clear the buffer
            int bytesRead = recv(sock, buffer, BUFFER_SIZE, 0);
            if (bytesRead <= 0) break;

            // split the buffer into command and arguments

            vector<Arg> args = tokenize(buffer);

            if (args.empty()) continue;

            string response = commandHandler.executeCommand(args[0].name, args.size(), args);
            send(sock, response.c_str(), response.length(), 0);
        }

        close(sock);
        return NULL;
    }
};
