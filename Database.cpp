#include "types.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

vector<string> splitString(const string &str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string token;

    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

class Database {
  private:
    string dbName;
    vector<Registro> records;
    pthread_mutex_t dbMutex;
    const string filename = "database.db";

    void loadFromFile() {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << "\n";
            return;
        }

        // Read records from file
        // Assuming the file is in CSV format
        string line;
        while (getline(file, line)) {
            Registro record;
            vector<string> fields = splitString(line, ',');
            if (fields.size() != 2) {
                cerr << "Invalid record format: " << line << "\n";
                continue;
            }
            record.id = stoi(fields[0]);
            record.nome = fields[1];
            records.push_back(record);
        }
        file.close();
    }

    void saveToFile() {
        ofstream file(filename, ios::trunc);
        for (const auto &record : records) {
            file << record.id << "," << record.nome << "\n";
        }
        file.close();
    }

  public:
    Database() {
        pthread_mutex_init(&dbMutex, NULL);
        loadFromFile();
    }

    ~Database() { pthread_mutex_destroy(&dbMutex); }

    string insertRecord(const Registro &registro) {
        pthread_mutex_lock(&dbMutex);
        auto it = find_if(records.begin(), records.end(), [registro](const Registro &record) { return record.id == registro.id; });
        if (it != records.end()) {
            pthread_mutex_unlock(&dbMutex);
            return "Error: Duplicate ID\n";
        }

        records.push_back(registro);
        saveToFile();
        pthread_mutex_unlock(&dbMutex);
        return "Record inserted successfully\n";
    }

    string deleteRecord(const int id) {
        pthread_mutex_lock(&dbMutex);
        auto it = find_if(records.begin(), records.end(), [id](const Registro &record) { return record.id == id; });
        if (it != records.end()) {
            records.erase(it, records.end());
            saveToFile();
            pthread_mutex_unlock(&dbMutex);
            return "Record deleted successfully\n";
        }
        pthread_mutex_unlock(&dbMutex);
        return "Record not found\n";
    }

    // string updateRecord(const string &oldRecord, const string &newRecord)
    // {
    //     string result;
    //     pthread_mutex_lock(&dbMutex);
    //     auto it = find(records.begin(), records.end(), oldRecord);
    //     if (it != records.end())
    //     {
    //         *it = newRecord;
    //         saveToFile();
    //         pthread_mutex_unlock(&dbMutex);
    //         result = "Record updated successfully\n";
    //     }
    //     else
    //     {
    //         result = "Record not found\n";
    //     }
    //     pthread_mutex_unlock(&dbMutex);
    //     return result;
    // }

    // string selectRecord(const Arg &arg)
    // {
    //     pthread_mutex_lock(&dbMutex);
    //     auto it = find(records.begin(), records.end(), record);
    //     pthread_mutex_unlock(&dbMutex);
    //     return (it != records.end()) ? "Record found: " + *it + "\n" : "Record not found\n";
    // }

    // string selectAllRecords()
    // {
    //     pthread_mutex_lock(&dbMutex);
    //     string result;
    //     for (const auto &record : records)
    //     {
    //         result += record + "\n";
    //     }
    //     pthread_mutex_unlock(&dbMutex);
    //     return result.empty() ? "No records found\n" : result;
    // }
};