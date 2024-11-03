#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_VARS 100 

struct var {
    char *str;   
    int global;  
};

struct var variables[MAX_VARS]; 
int var_count = 0; 

void set_variable(const char *name, const char *value, int global) {
    if (var_count >= MAX_VARS) {
        printf("Variable storage full\n");
        return;
    }
    variables[var_count].str = (char *)malloc(strlen(name) + strlen(value) + 2); 
    if (!variables[var_count].str) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    sprintf(variables[var_count].str, "%s=%s", name, value);
    variables[var_count].global = global; 
    var_count++;
}

char *get_variable(const char *name) {
    for (int i = 0; i < var_count; i++) {
        char *equal_sign = strchr(variables[i].str, '=');
        if (equal_sign) {
            int name_length = equal_sign - variables[i].str;
            if (strncmp(variables[i].str, name, name_length) == 0 && name_length == strlen(name)) {
                return equal_sign + 1; 
            }
        }
    }
    return NULL; 
}

void list_variables() {
    for (int i = 0; i < var_count; i++) {
        printf("%s\n", variables[i].str);
    }
}

int main() {
    char command[MAX_COMMAND_LENGTH];

    while (1) {
        printf("myshell> ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break; 
        }

        command[strcspn(command, "\n")] = 0; 

        if (strncmp(command, "set", 3) == 0) {
            char *var_part = command + 4; 
            char *equal_sign = strchr(var_part, '=');
            if (equal_sign) {
                *equal_sign = '\0'; 
                set_variable(var_part, equal_sign + 1, 0);
            } else {
                printf("Invalid syntax. Use: set name=value\n");
            }
        } else if (strncmp(command, "setglobal", 9) == 0) {
            char *var_part = command + 10; 
            char *equal_sign = strchr(var_part, '=');
            if (equal_sign) {
                *equal_sign = '\0'; 
                set_variable(var_part, equal_sign + 1, 1); 
            } else {
                printf("Invalid syntax. Use: setglobal name=value\n");
            }
        } else if (strncmp(command, "get", 3) == 0) {
            char *var_name = command + 4;
            char *value = get_variable(var_name);
            if (value) {
                printf("%s\n", value);
            } else {
                printf("Variable not found\n");
            }
        } else if (strcmp(command, "list") == 0) {
            list_variables();
        } else if (strncmp(command, "cd", 2) == 0) {
            char *path = command + 3; 
            if (chdir(path) != 0) {
                perror("chdir failed");
            }
        } else if (strcmp(command, "exit") == 0) {
            break;
        } else {
            printf("External command execution is not implemented in this example.\n");
        }
    }
    for (int i = 0; i < var_count; i++) {
        free(variables[i].str);
    }

    return 0;
}
