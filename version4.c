#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_HISTORY 10
#define MAX_COMMAND_LENGTH 1024

char *history[MAX_HISTORY];
int history_count = 0;

void add_to_history(char *command) {
    if (history_count == MAX_HISTORY) {
        free(history[0]);
        for (int i = 1; i < MAX_HISTORY; i++) {
            history[i - 1] = history[i];
        }
        history_count--;
    }
    history[history_count++] = strdup(command);
}

char *get_command_from_history(int index) {
    if (index < 0 || index >= history_count) {
        printf("No such command in history.\n");
        return NULL;
    }
    return history[index];
}

void display_prompt() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("PUCITshell@%s:- ", cwd);
}

void execute_command(char *command) {
    pid_t pid = fork();
    if (pid == 0) {
        char *args[MAX_COMMAND_LENGTH / 2 + 1];
        int i = 0;
        char *token = strtok(command, " ");
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;
        if (execvp(args[0], args) == -1) {
            perror("Command execution failed");
        }
        exit(1);
    } else if (pid > 0) {
        wait(NULL);
    } else {
        perror("Fork failed");
    }
}

int main() {
    char command[MAX_COMMAND_LENGTH];

    while (1) {
        display_prompt();

        if (fgets(command, MAX_COMMAND_LENGTH, stdin) == NULL) {
            break; 
        }

        command[strcspn(command, "\n")] = 0; 
        if (strlen(command) == 0) {
            continue;
        }

        if (command[0] == '!') {
            int cmd_index = (command[1] == '-') ? history_count - 1 : atoi(&command[1]) - 1;
            char *history_command = get_command_from_history(cmd_index);
            if (history_command) {
                printf("Executing: %s\n", history_command);
                add_to_history(history_command);
                execute_command(history_command);
            }
        } else {
            add_to_history(command);
            execute_command(command);
        }
    }

    for (int i = 0; i < history_count; i++) {
        free(history[i]);
    }

    return 0;
}
