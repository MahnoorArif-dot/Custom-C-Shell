#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>
#include <ctype.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_JOBS 100

typedef struct {
    pid_t pid;
    char command[MAX_COMMAND_LENGTH];
} Job;

Job jobs[MAX_JOBS];
int job_count = 0;

void add_job(pid_t pid, const char *command) {
    if (job_count < MAX_JOBS) {
        jobs[job_count].pid = pid;
        strncpy(jobs[job_count].command, command, MAX_COMMAND_LENGTH);
        job_count++;
    } else {
        printf("Job list full\n");
    }
}

void list_jobs() {
    if (job_count == 0) {
        printf("No background jobs.\n");
        return;
    }
    printf("Background jobs:\n");
    for (int i = 0; i < job_count; i++) {
        printf("[%d] PID: %d Command: %s\n", i + 1, jobs[i].pid, jobs[i].command);
    }
}

void kill_job(int job_number) {
    if (job_number < 1 || job_number > job_count) {
        printf("Invalid job number.\n");
        return;
    }
    pid_t pid = jobs[job_number - 1].pid;
    if (kill(pid, SIGKILL) == 0) {
        printf("Killed job [%d] PID: %d\n", job_number, pid);
    } else {
        perror("kill failed");
    }
}

void change_directory(char *path) {
    if (chdir(path) != 0) {
        perror("cd failed");
    }
}

void show_help() {
    printf("Built-in commands:\n");
    printf("cd <path>  : Change the current directory to <path>\n");
    printf("exit       : Exit the shell\n");
    printf("jobs       : List background jobs\n");
    printf("kill <job> : Kill job by its job number\n");
    printf("help       : Show this help message\n");
}

int main() {
    char command[MAX_COMMAND_LENGTH];

    while (1) {
        printf("myshell> ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break; 
        }

        command[strcspn(command, "\n")] = 0; 
        if (strncmp(command, "cd", 2) == 0) {
            char *path = command + 3; 
            change_directory(path);
        } else if (strcmp(command, "exit") == 0) {
            break;
        } else if (strcmp(command, "jobs") == 0) {
            list_jobs();
        } else if (strncmp(command, "kill", 4) == 0) {
            int job_number = atoi(command + 5); 
            kill_job(job_number);
        } else if (strcmp(command, "help") == 0) {
            show_help();
        } else {
            pid_t pid = fork();
            if (pid == 0) {
                char *args[100];
                char *token = strtok(command, " ");
                int i = 0;
                while (token != NULL) {
                    args[i++] = token;
                    token = strtok(NULL, " ");
                }
                args[i] = NULL; 
                execvp(args[0], args);
                perror("exec failed"); 
                exit(EXIT_FAILURE);
            } else if (pid > 0) {
                add_job(pid, command);
                waitpid(pid, NULL, 0); 
            } else {
                perror("fork failed");
            }
        }
    }

    return 0;
}
