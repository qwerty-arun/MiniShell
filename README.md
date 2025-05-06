# 🐚 MiniShell - A Basic Linux Shell in C++

## 📜 About
MiniShell is a simple Linux shell written in C++.
It supports running basic commands, handling directory changes (cd), and keeps a simple command history.
This project was built to deepen my understanding of system calls (fork(), execvp(), wait()) and Linux internals.

## 🚀 Features
Run standard Linux commands (ls, pwd, date, etc.)

Built-in cd command to change directories

Command history (view previous commands)

Colored and dynamic prompt

Basic error handling

## 🔧 Technologies Used
C++

Linux System Programming (fork, execvp, wait, chdir)

GCC Compiler

Git for version control

## 🛠️ How to Build and Run
Clone the repository:

bash
Copy
Edit
git clone https://github.com/yourusername/MiniShell.git
cd MiniShell
Compile the project:

bash
Copy
Edit
g++ main.cpp -o minishell
Run the shell:

bash
Copy
Edit
./minishell
📸 Demo
(You can add a screenshot or a gif later here!)
Example:

bash
Copy
Edit
MiniShell> pwd
/home/username/MiniShell
MiniShell> ls
main.cpp  minishell  README.md
MiniShell> cd ..
MiniShell> pwd
/home/username
MiniShell> exit
## 📚 What I Learned
How Linux creates and manages processes.

How command execution actually works under the hood.

Handling user input and system calls properly.

Building real-world C++ projects.

## 🤔 Future Improvements
Add support for piping (|) and redirection (>, <).

Implement tab-autocomplete for commands.

Add signal handling (Ctrl+C to terminate running processes, but not the shell).

Better memory management.

## 🧑‍💻 Author
Your Name

LinkedIn Profile

GitHub Profile

