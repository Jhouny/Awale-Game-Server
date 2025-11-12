#include "commander.h"

Response* execute_command(database *db, const Command* cmd, int client_socket_fd) {
    printf("Executing command: %s from client socket %d.\n", cmd->command, client_socket_fd);

    Response* res = (Response*) malloc(sizeof(Response));
    if (res == NULL) {
        printf("Error allocating response.\n");
        return NULL;
    }
    memset(res, 0, sizeof(Response));
    res->status_code = 0; // Default to failure
    res->body_size = 0;

    if (strcmp(cmd->command, "LOGIN") == 0) {
        if (cmd->args_size != 2) {
            printf("Invalid number of arguments for LOGIN command.\n");
            return res;
        }
        const char* username = cmd->args[0];
        const char* password = cmd->args[1];
        printf("Login attempt with username: %s and password: %s\n", username, password);

        // Verify user credentials against database
        table* users_table = get_table(db, "users");
        if (users_table == NULL) {
            printf("Users table not found in database.\n");
            return res;
        }
        char* stored_password = get(users_table, username);
        if (stored_password == NULL) {
            printf("Username %s not found.\n", username);
            return res;
        }
        if (strcmp(stored_password, password) != 0) {
            printf("Incorrect password for username %s.\n", username);
            return res;
        }
        printf("User %s successfully logged in.\n", username);

        res->status_code = 1; // Success
        return res;
    } else if(strcmp(cmd->command, "SIGNUP") == 0) {
        if (cmd->args_size != 2) {
            printf("Invalid number of arguments for SIGNUP command.\n");
            return res;
        }
        const char* username = cmd->args[0];
        const char* password = cmd->args[1];
        printf("Signup attempt with username: %s and password: %s\n", username, password);
    
        // Verify and add user to database
        table* users_table = get_table(db, "users");
        if (users_table == NULL) {
            printf("Users table not found in database.\n");
            return res;
        }

        if (get(users_table, username) != NULL) {
            printf("Username %s already exists.\n", username);
            return res;
        }
        if (!insert(users_table, username, password)) {
            printf("Error inserting new user into database.\n");
            return res;
        }
        printf("User %s successfully signed up.\n", username);
        res->status_code = 1; // Success
        return res;
    } else {
        printf("Unknown command: %s\n", cmd->command);
        return res;
    }
    return res;
}