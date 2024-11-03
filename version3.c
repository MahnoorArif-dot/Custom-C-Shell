#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>

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


void sigchld_handler(int sig) {
    int status;
    
    while (waitpid(-1, &status, WNOHANG) > 0);
}

void parse_input(char *input, char **args) {
    int i = 0;
    args[i] = strtok(input, " ");
    while (args[i] != NULL) {
        i++;
        args[i] = strtok(NULL, " ");
    }
}

int execute_command(char **args, int background) {
    pid_t pid = fork();
    if (pid == 0) {
        
        execvp(args[0], args);
        perror("execvp failed");
        exit(1);
    } else if (pid > 0) {
        
        if (background) {
            printf("[%d] %d\n", 1, pid);
            return 0;
        } else {
            waitpid(pid, NULL, 0);
        }
    } else {
        perror("fork failed");
        return 1;
    }
    return 0;
}

int main() {
    char input[MAX_INPUT_SIZE];
    char *args[MAX_ARGS];
    int background;
    signal(SIGCHLD, sigchld_handler);

    while (1) {
        display_prompt();

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\nExiting shell...\n");
            break;
        }

        input[strcspn(input, "\n")] = 0;

        
        parse_input(input, args);
        int argc = 0;
        while (args[argc] != NULL) argc++;

        background = 0;
        if (argc > 0 && strcmp(args[argc - 1], "&") == 0) {
            background = 1;
            args[argc - 1] = NULL;  
        }

        if (args[0] != NULL) {
            execute_command(args, background);
        }
    }

    return 0;
}
