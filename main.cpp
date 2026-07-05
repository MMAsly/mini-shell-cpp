#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
#include <cstring>

using namespace std;

vector<string> splitCommand(const string& input) {
    vector<string> parts;
    stringstream ss(input);
    string word;
    while (ss >> word) {
        parts.push_back(word);
    }
    return parts;
}

void executeCommand(vector<string> args) {
    vector<char*> cargs;
    for (auto& arg : args) {
        cargs.push_back(&arg[0]);
    }
    cargs.push_back(nullptr);

    pid_t pid = fork();

    if (pid == 0) {
        execvp(cargs[0], cargs.data());
        cerr << "Command not found: " << args[0] << endl;
        exit(1);
    } else if (pid > 0) {
        wait(nullptr);
    } else {
        cerr << "Fork failed." << endl;
    }
}

int main() {
    string input;
    vector<string> history;

    while (true) {
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        cout << "mini-shell:" << cwd << "$ ";

        getline(cin, input);

        if (input.empty()) {
            continue;
        }

        history.push_back(input);
        vector<string> args = splitCommand(input);

        if (args[0] == "exit") {
            break;
        } else if (args[0] == "cd") {
            if (args.size() < 2) {
                cerr << "cd: missing argument" << endl;
            } else if (chdir(args[1].c_str()) != 0) {
                cerr << "cd failed: " << strerror(errno) << endl;
            }
        } else if (args[0] == "pwd") {
            cout << cwd << endl;
        } else if (args[0] == "history") {
            for (size_t i = 0; i < history.size(); i++) {
                cout << i + 1 << ": " << history[i] << endl;
            }
        } else {
            executeCommand(args);
        }
    }

    return 0;
}
