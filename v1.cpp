#include <signal.h>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

void sigintHandler(int sig)
{
        cout << "\n\033[1;32mMiniShell>\033[0m "<<flush;
}
int main() {
    signal(SIGINT, sigintHandler);
    string input;
    vector<string> history;

    while (true) {
        cout << "\033[1;32mMiniShell>\033[0m ";
        if (!getline(cin, input)) break;

        if (input.empty()) continue;


        // Split input into tokens
        istringstream iss(input);
        vector<string> tokens;
        string token;
        while (iss >> token) tokens.push_back(token); //tokenization
        if (tokens.empty()) continue;

        // Handle "exit"
        if (tokens[0] == "exit") break;

        // Handle "cd"
        if (tokens[0] == "cd") {
            const char* path = (tokens.size() > 1) ? tokens[1].c_str() : getenv("HOME");
            if (chdir(path) != 0) perror("cd");
            continue;
        }

        const size_t HISTORY_LIMIT = 50;

        if(tokens[0]!="history")
        {
            history.push_back(input);
            if(history.size() > HISTORY_LIMIT)
                history.erase(history.begin()); //remove oldest
        }

        if(tokens[0] == "history")
        {
            int i = 1;
            for(auto command: history)
            {
                cout<<i++<<": "<<command<<endl;
            }
            continue;
        }

        if (tokens[0][0] == '!' && tokens[0].length() > 1) {
            int index = stoi(tokens[0].substr(1));
            if (index >= 1 && index <= history.size()) {
                input = history[index - 1];
                cout << input << endl;
                // re-tokenize from input again
                istringstream iss2(input);
                tokens.clear();
                while (iss2 >> token) tokens.push_back(token);
            } else {
                cout << "No such command in history.\n";
                continue;
            }
        }
        // Convert tokens to char*[]
        vector<char*> argv;
        for (auto& t : tokens) argv.push_back(const_cast<char*>(t.c_str()));
        argv.push_back(nullptr);

        // Fork and exec
        pid_t pid = fork();
        if (pid == 0) {
            execvp(argv[0], argv.data());
            cerr<<"Command not found: "<< argv[0] <<endl;
            exit(1);
        } else if (pid > 0) {
            wait(0);
        } else {
            cerr<<"Error: fork() failed to create a child process.\n"<<endl;
        }
    }

    return 0;
}
