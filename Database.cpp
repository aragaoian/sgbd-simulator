#include "BinaryTree.cpp"
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
    BST tree;
    vector<Registro> records;
    pthread_mutex_t dbMutex;
    pthread_mutex_t fileMutex;
    const string filename = "database.db";
    int operationsCount = 0;

    void loadFromFile() {
        pthread_mutex_lock(&fileMutex);
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
            tree.insert(record.id);
        }
        file.close();
        pthread_mutex_unlock(&fileMutex);
    }

    void saveToFile(bool force) {
        if (!force) {
            return;
        }
        operationsCount = 0;
        pthread_mutex_lock(&fileMutex);
        ofstream file(filename, ios::trunc);
        for (const auto &record : records) {
            file << record.id << "," << record.nome << "\n";
        }
        file.close();
        pthread_mutex_unlock(&fileMutex);
    }

  public:
    Database() {
        pthread_mutex_init(&dbMutex, NULL);
        pthread_mutex_init(&fileMutex, NULL);
        loadFromFile();
    }

    ~Database() {
        saveToFile(true);
        pthread_mutex_destroy(&dbMutex);
        pthread_mutex_destroy(&fileMutex);
    }

    string insertRecord(const Registro &registro) {
        pthread_mutex_lock(&dbMutex);

        // Check for duplicate ID
        bool inserted = tree.insert(registro.id);
        if (!inserted) {
            pthread_mutex_unlock(&dbMutex);
            return "Error: Duplicate ID\n";
        }

        records.push_back(registro);
        pthread_mutex_unlock(&dbMutex);
        saveToFile(false);
        return "Record inserted successfully\n";
    }

    string deleteRecord(const Arg &where) {
        pthread_mutex_lock(&dbMutex);

        if (where.name.empty()) {
            int numRecords = records.size();
            records.clear();
            tree.clear();
            pthread_mutex_unlock(&dbMutex);
            saveToFile(false);
            return "All records deleted successfully\n";
        }

        auto it = find_if(records.begin(), records.end(), [where](const Registro &record) {
            if (where.name == "id") {
                int id = stoi(where.value); // converte o id para inteiro
                return record.id == id;
            } else if (where.name == "nome") {
                return record.nome == where.value;
            }
            return false;
        });

        if (it != records.end()) {
            tree.remove(it->id);
            records.erase(it, records.end());
            pthread_mutex_unlock(&dbMutex);
            saveToFile(false);
            return "Record(s) deleted successfully\n";
        }
        pthread_mutex_unlock(&dbMutex);
        return "No records found to delete\n";
    }

    string updateRecord(const Arg &field, const Arg &where) {
        pthread_mutex_lock(&dbMutex);

        for (auto &record : records) {
            if (where.name == "id") {
                int id = stoi(where.value); // converte o id para inteiro
                if (record.id == id) {
                    record.nome = field.value;
                }
            } else {
                record.nome = field.value;
            }
        }
        pthread_mutex_unlock(&dbMutex);

        saveToFile(false);
        return "Record(s) updated successfully\n";
    }

    string selectRecord(const vector<Arg> &fields, const Arg &where) {
        pthread_mutex_lock(&dbMutex);
        vector<Registro> result;

        for (const auto &record : records) {
            if (!where.name.empty()) {
                if (where.name == "id") {
                    int id = stoi(where.value); // converte o id para inteiro
                    if (record.id == id) {
                        result.push_back(record);
                    }
                } else if (where.name == "nome") {
                    if (record.nome == where.value) {
                        result.push_back(record);
                    }
                }
            } else {
                result.push_back(record);
            }
        }
        pthread_mutex_unlock(&dbMutex);

        if (result.empty()) return "No records found\n";

        string resultStrTable;
        bool hasId = false;
        bool hasNome = false;
        for (const auto &field : fields) { // verifica os campos do select
            if (field.name == "id") {
                hasId = true;
            } else if (field.name == "nome") {
                hasNome = true;
            }
        }

        if (hasId && hasNome) {
            resultStrTable += "\n+---------+-----------------------------+\n";
            resultStrTable += "|   ID    |             Nome            |\n";
            resultStrTable += "+---------+-----------------------------+\n";
        } else if (hasId) {
            resultStrTable += "\n+---------+\n";
            resultStrTable += "|   ID    |\n";
            resultStrTable += "+---------+\n";
        } else if (hasNome) {
            resultStrTable += "\n+-----------------------------+\n";
            resultStrTable += "|             Nome            |\n";
            resultStrTable += "+-----------------------------+\n";
        } else {
            return "No fields to display\n";
        }

        for (const auto &record : result) {
            if (hasId && hasNome) {
                string idStr = to_string(record.id);
                string idPadded = string(4 - idStr.length() / 2, ' ') + idStr + string(4 - idStr.length() / 2, ' ');
                if (idStr.length() % 2 == 0) idPadded += " ";

                string namePadded = record.nome;
                int nameSpace = 29 - namePadded.length();
                int leftPad = nameSpace / 2;
                int rightPad = nameSpace - leftPad;
                namePadded = string(leftPad, ' ') + namePadded + string(rightPad, ' ');

                resultStrTable += "|" + idPadded + "|" + namePadded + "|\n";
            } else if (hasId) {
                string idStr = to_string(record.id);
                string idPadded = string(4 - idStr.length() / 2, ' ') + idStr + string(4 - idStr.length() / 2, ' ');
                if (idStr.length() % 2 == 0) idPadded += " ";
                resultStrTable += "|" + idPadded + "|\n";
            } else if (hasNome) {
                string namePadded = record.nome;
                int nameSpace = 29 - namePadded.length();
                int leftPad = nameSpace / 2;
                int rightPad = nameSpace - leftPad;
                namePadded = string(leftPad, ' ') + namePadded + string(rightPad, ' ');
                resultStrTable += "|" + namePadded + "|\n";
            }
        }

        if (hasId && hasNome) {
            resultStrTable += "+---------+-----------------------------+\n";
        } else if (hasId) {
            resultStrTable += "+---------+\n";
        } else if (hasNome) {
            resultStrTable += "+-----------------------------+\n";
        }

        return resultStrTable;
    }
};