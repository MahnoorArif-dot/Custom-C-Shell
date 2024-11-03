#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
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
        parse_input(input, args);


        if (strcmp(args[0], "exit") == 0) {
            break;
        }

        pid_t pid = fork();
        if (pid == 0) {
            execvp(args[0], args);
            perror("execvp failed"); 
            exit(1);
        } else if (pid > 0) {
            wait(NULL); 
        } else {
            perror("fork failed");
        }
    }

    return 0;
}
