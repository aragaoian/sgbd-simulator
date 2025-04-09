#include "commands.cpp"
#include "types.h"
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

class CommandHandler {
  public:
    using CommandFunction = string (*)(int, vector<Arg> &);

    string executeCommand(const string &name, int argc, vector<Arg> &argv) {
        if (commands.find(name) != commands.end()) {
            return commands[name](argc, argv);
        }
        return "Error: Command not found\n";
    }

  private:
    unordered_map<string, CommandFunction> commands = {
        {"insert", insert_cmd},
        // {"delete", delete_cmd},
        {"update", update_cmd},
        // {"select", select_cmd},
        // {"select_all", select_all_cmd},
        // {"help", help_cmd},
    };
};
