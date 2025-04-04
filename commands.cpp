#include "Database.cpp"
#include "types.h"
#include <string>

using namespace std;

Database db;

string insert_cmd(int argc, vector<Arg> &argv) {
    if (argc != 3) {
        return "Usage: insert id=<id> nome='<nome>'\n";
    }
    if (argv[1].name != "id" || argv[2].name != "nome") {
        return "Error: Invalid arguments\n";
    }
    int id = stoi(argv[1].value);
    string nome = argv[2].value;
    Registro record;
    record.id = id;
    record.nome = nome;
    return db.insertRecord(record);
}

string delete_cmd(int argc, vector<Arg> &argv) {
    if (argc != 2) {
        return "Usage: delete <record>\n";
    }
    return "Error: delete command not implemented\n";
    // string record = argv[1];
    // return db.deleteRecord(record);
}

// string update_cmd(int argc, vector<Arg> &argv) {
// {
//     if (argc != 3)
//     {
//         return "Usage: update <old_record> <new_record>\n";
//     }
//     string oldRecord = argv[1];
//     string newRecord = argv[2];
//     return db.updateRecord(oldRecord, newRecord);
// }

// string select_cmd(int argc, vector<Arg> &argv) {
// {
//     if (argc != 2)
//     {
//         return "Usage: select <record>\n";
//     }
//     string record = argv[1];
//     return db.selectRecord(record);
// }

// string select_all_cmd(int argc, vector<Arg> &argv) {
// {
//     if (argc != 1)
//     {
//         return "Usage: select_all\n";
//     }
//     return db.selectAllRecords();
// }

string help_cmd(int argc, vector<Arg> &argv) { return "Available commands: insert, delete, update, select, help\n"; }
