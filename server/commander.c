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
    res->message_size = 0;
    res->body_size = 0;

    if (strcmp(cmd->command, "LOGIN") == 0) {
        if (cmd->args_size != 2) {
            printf("Invalid number of arguments for LOGIN command.\n");
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for LOGIN.");
            return res;
        }
        const char* username = cmd->args[0];
        const char* password = cmd->args[1];
        printf("Login attempt with username: %s and password: %s\n", username, password);

        // Verify user credentials against database
        table* users_table = get_table(db, "users", 0);
        if (users_table == NULL) {
            printf("Users table not found in database.\n");
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Users table not found.");
            return res;
        }
        char* stored_password = get(users_table, username);
        if (stored_password == NULL) {
            printf("Username %s not found.\n", username);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Username not found.");
            return res;
        }
        if (strcmp(stored_password, password) != 0) {
            printf("Incorrect password for username %s.\n", username);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Password incorrect.");
            return res;
        }
        printf("User %s successfully logged in.\n", username);

        res->status_code = 1; // Success
        return res;
    } else if(strcmp(cmd->command, "SIGNUP") == 0) {
        if (cmd->args_size != 2) {
            printf("Invalid number of arguments for SIGNUP command.\n");
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for SIGNUP.");
            return res;
        }
        const char* username = cmd->args[0];
        const char* password = cmd->args[1];
        printf("Signup attempt with username: %s and password: %s\n", username, password);
    
        // Verify and add user to database
        table* users_table = get_table(db, "users", 0);
        if (users_table == NULL) {
            printf("Users table not found in database.\n");
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Table 'users' not found in database.");
            return res;
        }

        if (get(users_table, username) != NULL) {
            printf("Username %s already exists.\n", username);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Username is already taken.");
            return res;
        }
        if (!insert(users_table, username, password)) {
            printf("Error inserting new user into database.\n");
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Couldn't insert new user into database.");
            return res;
        }
        printf("User %s successfully signed up.\n", username);
        res->status_code = 1; // Success
        return res;
    } else if (strcmp(cmd->command, "RETRIEVE_CHALLENGES") == 0) {
        if (cmd->args_size != 0) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
        // Retrieve challenges addressed to the user from the database
    } else if (strcmp(cmd->command, "CHALLENGE") == 0) {
        if (cmd->args_size != 2) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
    } else if (strcmp(cmd->command, "RETRIEVE_BIO") == 0) {
        if (cmd->args_size != 1) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
    } else if (strcmp(cmd->command, "WRITE_BIO") == 0) {
        if (cmd->args_size != 1) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
    } else if (strcmp(cmd->command, "RETRIEVE_GAMES") == 0) {
        if (cmd->args_size != 0) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
    } else if (strcmp(cmd->command, "SPECTATE") == 0) {
        if (cmd->args_size != 1) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
    } else if (strcmp(cmd->command, "RETRIEVE_SELF_GAMES") == 0) {
        if (cmd->args_size != 0) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
    } else if (strcmp(cmd->command, "REVIEW") == 0) {
        if (cmd->args_size != 1) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
    } else if (strcmp(cmd->command, "RETRIEVE_CHATS") == 0) {
        if (cmd->args_size != 0) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
    } else if (strcmp(cmd->command, "OPEN_CHAT") == 0) {
        if (cmd->args_size != 1) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
    } else if (strcmp(cmd->command, "SEND_MSG") == 0) {
        if (cmd->args_size != 2) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
    } else if (strcmp(cmd->command, "CREATE_CHAT") == 0) {
        if (cmd->args_size != 1) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
    } else if (strcmp(cmd->command, "RETRIEVE_FRIENDS") == 0) {
        if (cmd->args_size != 0) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
    } else if (strcmp(cmd->command, "ADD_FRIEND") == 0) {
        if (cmd->args_size != 1) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
    } else if (strcmp(cmd->command, "REMOVE_FRIEND") == 0) {
        if (cmd->args_size != 1) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
    } else {
        printf("Unknown command: %s\n", cmd->command);
        res->message_size = snprintf(res->message, MAX_ARG_LEN, "Unknown command: %s", cmd->command);
        return res;
    }
    return res;
}