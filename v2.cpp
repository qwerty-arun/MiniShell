#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>      // for open()
#include <signal.h>     // for signal handling
using namespace std;

const size_t HISTORY_LIMIT = 50;

// Handler for Ctrl+C
void sigintHandler(int sig) {
    cout << "\n\033[1;32mMiniShell>\033[0m " << flush;
}

// Helper function: split string by delimiter
vector<string> splitByDelimiter(const string& input, char delimiter) {
    vector<string> result;
    stringstream ss(input);
    string segment;
    while (getline(ss, segment, delimiter)) {
        result.push_back(segment);
    }
    return result;
}

// Executes a command with optional redirection
void executeCommandWithRedirection(vector<string>& tokens) {
    int fd_in = -1, fd_out = -1;

    // Handle redirection symbols and open files
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i] == "<" && i + 1 < tokens.size()) {
            fd_in = open(tokens[i + 1].c_str(), O_RDONLY);
            if (fd_in < 0) perror("open for input");
            tokens.erase(tokens.begin() + i, tokens.begin() + i + 2);
            i--;
        } else if (tokens[i] == ">" && i + 1 < tokens.size()) {
            fd_out = open(tokens[i + 1].c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out < 0) perror("open for output");
            tokens.erase(tokens.begin() + i, tokens.begin() + i + 2);
            i--;
        } else if (tokens[i] == ">>" && i + 1 < tokens.size()) {
            fd_out = open(tokens[i + 1].c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd_out < 0) perror("open for append");
            tokens.erase(tokens.begin() + i, tokens.begin() + i + 2);
            i--;
        }
    }

    // Convert to char* array
    vector<char*> argv;
    for (auto& t : tokens) argv.push_back(const_cast<char*>(t.c_str()));
    argv.push_back(nullptr);

    // Apply redirection
    if (fd_in != -1) {
        dup2(fd_in, STDIN_FILENO);
        close(fd_in);
    }
    if (fd_out != -1) {
        dup2(fd_out, STDOUT_FILENO);
        close(fd_out);
    }

    // Execute the command
    execvp(argv[0], argv.data());
    perror("execvp failed");
    exit(1);
}

// Executes piped commands
void executePipedCommands(const vector<string>& commands) {
    int num_cmds = commands.size();
    int prev_fd = -1;

    for (int i = 0; i < num_cmds; ++i) {
        int pipefd[2];
        if (i < num_cmds - 1) {
            if (pipe(pipefd) == -1) {
                perror("pipe");
                return;
            }
        }

        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            if (i > 0) {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }

            if (i < num_cmds - 1) {
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[0]);
                close(pipefd[1]);
            }

            // Tokenize individual command
            istringstream iss(commands[i]);
            vector<string> tokens;
            string token;
            while (iss >> token) tokens.push_back(token);

            // Call redirection-aware executor
            executeCommandWithRedirection(tokens);
        } else if (pid > 0) {
            // Parent process
            if (prev_fd != -1) close(prev_fd);
            if (i < num_cmds - 1) {
                close(pipefd[1]);
                prev_fd = pipefd[0];
            }
            wait(0); // Wait for child
        } else {
            perror("fork failed");
        }
    }
}

int main() {
    signal(SIGINT, sigintHandler);
    string input;
    vector<string> history;

    while (true) {
        cout << "\033[1;32mMiniShell>\033[0m ";
        if (!getline(cin, input)) break;
        if (input.empty()) continue;

        // Save to history (if not 'history' itself)
        istringstream iss_check(input);
        string cmd_check;
        iss_check >> cmd_check;
        if (cmd_check != "history") {
            history.push_back(input);
            if (history.size() > HISTORY_LIMIT)
                history.erase(history.begin());
        }

        // Repeat command using !N
        if (input[0] == '!' && input.length() > 1) {
            int index = stoi(input.substr(1));
            if (index >= 1 && index <= history.size()) {
                input = history[index - 1];
                cout << input << endl;
            } else {
                cout << "No such command in history.\n";
                continue;
            }
        }

        // Tokenize input
        istringstream iss(input);
        vector<string> tokens;
        string token;
        while (iss >> token) tokens.push_back(token);
        if (tokens.empty()) continue;

        // Built-in commands
        if (tokens[0] == "exit") break;
        if (tokens[0] == "cd") {
            const char* path = (tokens.size() > 1) ? tokens[1].c_str() : getenv("HOME");
            if (chdir(path) != 0) perror("cd");
            continue;
        }
        if (tokens[0] == "history") {
            int i = 1;
            for (const auto& cmd : history) {
                cout << i++ << ": " << cmd << endl;
            }
            continue;
        }

        // Handle pipes
        if (input.find('|') != string::npos) {
            vector<string> pipedCommands = splitByDelimiter(input, '|');
            executePipedCommands(pipedCommands);
            continue;
        }

        // Handle single commands with redirection
        pid_t pid = fork();
        if (pid == 0) {
            executeCommandWithRedirection(tokens);
        } else if (pid > 0) {
            wait(0);
        } else {
            cerr << "Error: fork() failed.\n";
        }
    }

    return 0;
}
