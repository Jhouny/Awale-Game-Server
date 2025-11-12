#include "commander.h"

int execute_command(database *db, const Command* cmd, int client_socket_fd) {
    printf("Executing command: %s from client socket %d.\n", cmd->command, client_socket_fd);

    if (strcmp(cmd->command, "LOGIN") == 0) {
        if (cmd->args_size != 2) {
            printf("Invalid number of arguments for LOGIN command.\n");
            return -1;
        }
        const char* username = cmd->args[0];
        const char* password = cmd->args[1];
        printf("Login attempt with username: %s and password: %s\n", username, password);
        
    } else if(strcmp(cmd->command, "SIGNUP") == 0) {
        if (cmd->args_size != 2) {
            printf("Invalid number of arguments for SIGNUP command.\n");
            return -1;
        }
        const char* username = cmd->args[0];
        const char* password = cmd->args[1];
        printf("Signup attempt with username: %s and password: %s\n", username, password);
    
        // Verify and add user to database
        table* users_table = get_table(db, "users");
        if (users_table == NULL) {
            printf("Users table not found in database.\n");
            return -1;
        }

        if (get(users_table, username) != NULL) {
            printf("Username %s already exists.\n", username);
            return -1;
        }
        if (!insert(users_table, username, password)) {
            printf("Error inserting new user into database.\n");
            return -1;
        }
        printf("User %s successfully signed up.\n", username);
    } else {
        printf("Unknown command: %s\n", cmd->command);
        return -1;
    }
    return 0;
}