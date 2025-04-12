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
    if (argc != 1 && (argc != 3 || argv[1].name != "where")) {
        return "Usage: delete or delete where id=<id>\n";
    }

    return db.deleteRecord(argc == 3 ? argv[1] : Arg{});
}

string update_cmd(int argc, vector<Arg> &argv) {

    if (argc != 2 && (argc != 4 || argv[2].name != "where")) {
        return "Usage: update nome=<name> or update nome=<name> where id=<id>\n";
    }

    return db.updateRecord(argv[1], argc == 4 ? argv[3] : Arg{});
}

string select_cmd(int argc, vector<Arg> &argv) {
    if (argc < 2) {
        return "Usage: select <fields> where <condition>\n";
    }
    vector<Arg> fields;
    Arg where;

    for (int i = 1; i < argc; ++i) {
        if (argv[i].name == "where") {
            if (i + 1 >= argc) {
                return "Error: Missing condition after 'where'\n";
            }
            if (argv[i + 1].name != "id" && argv[i + 1].name != "nome") {
                return "Error: Invalid condition\n";
            }
            where = argv[i + 1];
            break;
        }
        if (argv[i].name != "id" && argv[i].name != "nome") {
            return "Error: Invalid field name\n";
        }
        if (!argv[i].value.empty()) {
            return "Error: Invalid field value - " + argv[i].name + "\n";
        }
        fields.push_back(argv[i]);
    }

    return db.selectRecord(fields, where);
}

string help_cmd(int argc, vector<Arg> &argv) { return "Available commands: insert, delete, update, select, help\n"; }
