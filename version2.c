#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pwd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64

void display_prompt() {
    char cwd[1024];
    char hostname[1024];
    struct passwd *pw = getpwuid(getuid());

    getcwd(cwd, sizeof(cwd));
    gethostname(hostname, sizeof(hostname));

    printf("%s@%s: %s PUCITshell:- ", pw->pw_name, hostname, cwd);
    fflush(stdout);
}

void parse_input(char *input, char **args) {
    int i = 0;
    args[i] = strtok(input, " ");
    while (args[i] != NULL) {
        i++;
        args[i] = strtok(NULL, " ");
    }
}

int execute_command(char **args) {
    int in_fd = -1, out_fd = -1;
    char *command[MAX_ARGS];
    int cmd_index = 0, i = 0;
    while (args[i] != NULL) {
        if (strcmp(args[i], "<") == 0) {
            in_fd = open(args[i + 1], O_RDONLY);
            if (in_fd < 0) {
                perror("Input file open error");
                return 1;
            }
            i += 2;
        } else if (strcmp(args[i], ">") == 0) {
            out_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (out_fd < 0) {
                perror("Output file open error");
                return 1;
            }
            i += 2;
        } else {
            command[cmd_index++] = args[i++];
        }
    }
    command[cmd_index] = NULL;

    pid_t pid = fork();
    if (pid == 0) {
        if (in_fd != -1) {
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }
        if (out_fd != -1) {
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }
        execvp(command[0], command);
        perror("execvp failed");
        exit(1);
    } else if (pid > 0) {
        wait(NULL);
    } else {
        perror("fork failed");
        return 1;
    }
    return 0;
}

int execute_piped_command(char *input) {
    char *commands[2];
    char *args1[MAX_ARGS], *args2[MAX_ARGS];

    commands[0] = strtok(input, "|");
    commands[1] = strtok(NULL, "|");

    if (commands[1] == NULL) {
        return execute_command(args1);
    }

    parse_input(commands[0], args1);
    parse_input(commands[1], args2);

    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("pipe failed");
        return 1;
    }

    pid_t pid1 = fork();
    if (pid1 == 0) {
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        execvp(args1[0], args1);
        perror("execvp failed");
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        dup2(pipe_fd[0], STDIN_FILENO);
        close(pipe_fd[1]);
        close(pipe_fd[0]);
        execvp(args2[0], args2);
        perror("execvp failed");
        exit(1);
    }

    close(pipe_fd[0]);
    close(pipe_fd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}

int main() {
    char input[MAX_INPUT_SIZE];
    char *args[MAX_ARGS];

    while (1) {
        display_prompt();

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\nExiting shell...\n");
            break;
        }

        input[strcspn(input, "\n")] = 0;

        if (strchr(input, '|') != NULL) {
            execute_piped_command(input);
        } else {
            parse_input(input, args);
            execute_command(args);
        }
    }

    return 0;
}
