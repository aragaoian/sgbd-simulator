#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <thread>
#include <unistd.h>
using namespace std;

#define BUFFER_SIZE 1500

class Client {
  public:
    Client(int port, const char *hostName) : port(port), hostName(hostName) {}

    void start() {
        if (serverConnect()) {
            sendData();
        }
    }

  private:
    int port;
    int serverSd;
    string hostName;
    sockaddr_in servAddr;
    struct hostent *server; // hostent to store host info
    char buffer[BUFFER_SIZE];

    bool serverConnect() {
        serverSd = socket(AF_INET, SOCK_STREAM, 0); // socket ID
        if (serverSd < 0) {
            cerr << "Error establishing the server socket" << endl;
            exit(EXIT_FAILURE);
            return false;
        }

        server = gethostbyname(hostName.c_str());
        if (server == NULL) {
            cerr << "Host not found" << endl;
            exit(EXIT_FAILURE);
            return false;
        }

        bzero((char *)&servAddr, sizeof(servAddr));
        servAddr.sin_family = AF_INET;
        memcpy(&servAddr.sin_addr.s_addr, server->h_addr, server->h_length); // copy server IP adress
        servAddr.sin_port = htons(port);

        if (connect(serverSd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
            cerr << "Error connecting to the server socket" << endl;
            exit(EXIT_FAILURE);
            return false;
        }

        return true;
    }

    void sendData() {
        bzero(buffer, BUFFER_SIZE);
        fgets(buffer, BUFFER_SIZE - 1, stdin);

        int wordLenght = write(serverSd, buffer, strlen(buffer));
        if (wordLenght <= 0) {
            cerr << "Error writing to the server socket" << endl;
        }
        bzero(buffer, BUFFER_SIZE);

        int responseLenght = read(serverSd, buffer, BUFFER_SIZE - 1);
        if (responseLenght <= 0) {
            cerr << "Error reading from the server socket" << endl;
        }

        cout << "Server: " << buffer << endl;
        close(serverSd);
    }
};
