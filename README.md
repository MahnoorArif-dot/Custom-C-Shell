🧠 Custom Linux Shell in C
This project involves the development of a custom shell for Linux systems, written in C language. The shell is built incrementally over six versions, with each version introducing new functionalities to better understand how real-world shells like Bash work.

The primary goal is to explore process management, command parsing, I/O handling, and system-level programming concepts through hands-on implementation.

📦 Version Descriptions
🔹 Version 1: Basic Shell
Simple REPL (Read-Eval-Print Loop)

Accepts user input and executes basic system commands (e.g., ls, pwd, date)

Uses fork() and execvp() to spawn child processes

🔹 Version 2: Command Execution with Arguments
Supports commands with multiple arguments (e.g., ls -l -a)

Parses and tokenizes input strings

🔹 Version 3: Background Process Handling
Allows execution of background processes using & (e.g., sleep 10 &)

Uses waitpid() with WNOHANG to prevent blocking

🔹 Version 4: Input and Output Redirection
Handles >, >> for output redirection and < for input redirection

Uses dup2() to manage file descriptors

🔹 Version 5: Piping Commands
Implements piping with | (e.g., ls -l | grep txt)

Creates pipes between multiple child processes

🔹 Version 6: Signal Handling and Built-in Commands
Handles signals like Ctrl+C (SIGINT) and Ctrl+Z (SIGTSTP)

Adds basic built-in commands such as:

cd

exit

history (optional)

Improves prompt formatting and user experience

🛠️ Technologies Used
Language: C

Platform: Linux / Unix

Compiler: GCC

System Calls: fork(), execvp(), wait(), dup2(), pipe(), signal(), etc.
