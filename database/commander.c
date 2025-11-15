#include "commander.h"

commanderGlobals cmdGlobals;

Response* execute_command(const Command* cmd, int client_socket_fd) {
    printf("Executing command: %s from client socket %d.\n", cmd->command, client_socket_fd);

	char fd_string[MAX_VALUE_LEN];
	snprintf(fd_string, MAX_VALUE_LEN, "%d", client_socket_fd);

    Response* res = (Response*) malloc(sizeof(Response));
    if (res == NULL) {
        printf("Error allocating response.\n");
        return NULL;
    }
    memset(res, 0, sizeof(Response));
    res->status_code = 0; // Default to failure
    res->message_size = 0;
    res->body_size = 0;

    if (strcmp(cmd->command, "INSERT") == 0) {  // Args: table name, key, value
        if (cmd->args_size != 3) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
        const char* table_name = cmd->args[0];
        const char* key = cmd->args[1];
        const char* value = cmd->args[2];
        table* target_table = get_table(cmdGlobals.db, table_name, 0);
        if (target_table == NULL) {
            printf("Table %s not found in database.\n", table_name);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Table %s not found.", table_name);
            return res;
        }
        if (!insert(target_table, key, value)) {
            printf("Error inserting entry into table %s.\n", table_name);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Couldn't insert entry into table %s. Check if the key already exists.", table_name);
            return res;
        }
        printf("Successfully inserted entry into table %s: (%s, %s).\n", table_name, key, value);
        res->status_code = 1; // Success
        return res;
    } else if(strcmp(cmd->command, "UPDATE") == 0) {  // Args: table name, key, new value
        if (cmd->args_size != 3) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
        const char* table_name = cmd->args[0];
        const char* key = cmd->args[1];
        const char* new_value = cmd->args[2];
        table* target_table = get_table(cmdGlobals.db, table_name, 0);
        if (target_table == NULL) {
            printf("Table %s not found in database.\n", table_name);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Table %s not found.", table_name);
            return res;
        }
        if (!update(target_table, key, new_value)) {
            printf("Error updating entry in table %s.\n", table_name);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Couldn't update entry in table %s. Check if the key exists.", table_name);
            return res;
        }
        printf("Successfully updated entry in table %s: (%s, %s).\n", table_name, key, new_value);
        res->status_code = 1; // Success
        return res;
    } else if(strcmp(cmd->command, "DELETE") == 0) {  // Args: table name, key
        if (cmd->args_size != 2) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
        const char* table_name = cmd->args[0];
        const char* key = cmd->args[1];
        table* target_table = get_table(cmdGlobals.db, table_name, 0);
        if (target_table == NULL) {
            printf("Table %s not found in database.\n", table_name);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Table %s not found.", table_name);
            return res;
        }
        if (!remove_entry(target_table, key)) {
            printf("Error deleting entry from table %s.\n", table_name);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Couldn't delete entry from table %s. Check if the key exists.", table_name);
            return res;
        }
        printf("Successfully deleted entry from table %s with key %s.\n", table_name, key);
        res->status_code = 1; // Success
        return res;
    } else if(strcmp(cmd->command, "GET") == 0) {  // Args: table name, key
        if (cmd->args_size != 2) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
        const char* table_name = cmd->args[0];
        const char* key = cmd->args[1];
        table* target_table = get_table(cmdGlobals.db, table_name, 0);
        if (target_table == NULL) {
            printf("Table %s not found in database.\n", table_name);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Table %s not found.", table_name);
            return res;
        }
        char* value = get(target_table, key);
        if (value == NULL) {
            printf("Key %s not found in table %s.\n", key, table_name);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Key %s not found in table %s.", key, table_name);
            return res;
        }
        printf("Successfully retrieved entry from table %s: (%s, %s).\n", table_name, key, value);
        res->status_code = 1; // Success
        res->body_size = 1;
        res->body[0] = (char*) malloc(MAX_ARG_LEN * sizeof(char));
        if (res->body[0] == NULL) {
            printf("Error allocating memory for value in response body.\n");
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Couldn't allocate memory for value.");
            return res;
        }
        strcpy(res->body[0], value);
        return res;
    } else if(strcmp(cmd->command, "GET_ALL") == 0) {  // Args: table name
        if (cmd->args_size != 1) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
        const char* table_name = cmd->args[0];
        table* target_table = get_table(cmdGlobals.db, table_name, 0);
        if (target_table == NULL) {
            printf("Table %s not found in database.\n", table_name);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Table %s not found.", table_name);
            return res;
        }
        int body_index = 0;
        char** all_entries = get_all_keys(target_table, &body_index);
        if (all_entries == NULL || body_index == 0) {
            printf("No entries found in table %s.\n", table_name);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "No entries found in table %s.", table_name);
            return res;
        }
        for (int i = 0; i < body_index; i++) {
            res->body[i] = all_entries[i];
        }
        free(all_entries);
        res->body_size = body_index;
        printf("Successfully retrieved all entries from table %s. Total entries: %d.\n", table_name, res->body_size);
        res->status_code = 1; // Success
        return res;
    } else {
        printf("Unknown command: %s\n", cmd->command);
        res->message_size = snprintf(res->message, MAX_ARG_LEN, "Unknown command: %s", cmd->command);
        return res;
    }
    return res;
}
