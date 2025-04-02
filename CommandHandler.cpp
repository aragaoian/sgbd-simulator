#include <unordered_map>
#include <string>
#include "commands.cpp"
using namespace std;

class CommandHandler
{
public:
    using CommandFunction = string (*)(int, char **);

    string executeCommand(const string &name, int argc, char **argv)
    {
        if (commands.find(name) != commands.end())
        {
            return commands[name](argc, argv);
        }
        return "Error: Command not found\n";
    }

private:
    unordered_map<string, CommandFunction> commands = {
        {"insert", insert_cmd},
    };
};
