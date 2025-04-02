#include <vector>
#include <string>
#include <algorithm>
using namespace std;

class Database
{
private:
    string dbName;
    vector<string> records;
    pthread_mutex_t dbMutex;

public:
    Database()
    {
        pthread_mutex_init(&dbMutex, NULL);
    }

    ~Database()
    {
        pthread_mutex_destroy(&dbMutex);
    }

    string insertRecord(const string record)
    {
        pthread_mutex_lock(&dbMutex);
        records.push_back(record);
        pthread_mutex_unlock(&dbMutex);
        return "Record inserted successfully\n";
    }

    string deleteRecord(const string record)
    {
        auto it = find(records.begin(), records.end(), record);
        if (it != records.end())
        {
            pthread_mutex_lock(&dbMutex);
            records.erase(it);
            pthread_mutex_unlock(&dbMutex);
            return "Record deleted successfully\n";
        }
        else
        {
            return "Record not found\n";
        }
    }
    string updateRecord(const string oldRecord, const string newRecord)
    {
        auto it = find(records.begin(), records.end(), oldRecord);
        if (it != records.end())
        {
            pthread_mutex_lock(&dbMutex);
            *it = newRecord;
            pthread_mutex_unlock(&dbMutex);
            return "Record updated successfully\n";
        }
        else
        {
            return "Record not found\n";
        }
    }
    string selectRecord(const string record)
    {
        auto it = find(records.begin(), records.end(), record);
        if (it != records.end())
        {
            return "Record found: " + *it + "\n";
        }
        else
        {
            return "Record not found\n";
        }
    }
    string selectAllRecords()
    {
        string result;
        for (const auto &record : records)
        {
            result += record + "\n";
        }
        return result.empty() ? "No records found\n" : result;
    }
};