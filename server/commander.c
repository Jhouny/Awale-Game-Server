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
        table* users_table = get_table(cmdGlobals.db, "users", 0);
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

		// Associate file_descriptor with username
		insert(cmdGlobals.user_fds, username, fd_string);

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
        table* users_table = get_table(cmdGlobals.db, "users", 0);
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
		char* username = get(cmdGlobals.user_fds, fd_string);
		if (username == NULL) {
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "No registered username for fd %d.\n", client_socket_fd);
			return res;
		}

		char* challenges_list = get(cmdGlobals.challenges, username);  // Retrieve challenges addressed to the username
		if (challenges_list == NULL)
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "No challenges found.");
		else {
			// Split the value retrieved by commas (,)
			for (int i = 0; i < MAX_VALUE_LEN; i++) {
				char currUsername[MAX_ARG_LEN];
				int currIndex = 0;
				while (challenges_list[i] != ',') {
					currUsername[currIndex++] = challenges_list[i];
				}
                strcpy(res->body[res->body_size++], currUsername);
                if (challenges_list[i] == '\0')
                    break;
                i++; // Skip the comma
			}
		}
		res->status_code = 1;
		return res;

    } else if (strcmp(cmd->command, "CHALLENGE") == 0) {
        if (cmd->args_size != 2) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }

        const char* challenged_username = cmd->args[0];
        const char* mode = cmd->args[1]; // public or private
        char* challenger_username = get(cmdGlobals.user_fds, fd_string);
        if (challenger_username == NULL) {
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "No registered username for fd %d.\n", client_socket_fd);
            return res;
        }
        // Add challenge to the challenges table
        char* existing_challenges = get(cmdGlobals.challenges, challenged_username);
        char new_challenge_entry[MAX_VALUE_LEN];
        if (existing_challenges == NULL)
            snprintf(new_challenge_entry, MAX_VALUE_LEN, "%s", challenger_username);
        else
            snprintf(new_challenge_entry, MAX_VALUE_LEN, "%s,%s", existing_challenges, challenger_username);
        
        if (!insert(cmdGlobals.challenges, challenged_username, new_challenge_entry)) {
            printf("Error inserting challenge into challenges table.\n");
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Couldn't insert challenge into database.");
            return res;
        }
        printf("User %s is challenging %s in %s mode.\n", challenger_username, challenged_username, mode);
        res->status_code = 1; // Success
        return res;
    } else if (strcmp(cmd->command, "RETRIEVE_BIO") == 0) {
        if (cmd->args_size != 1) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
        const char* target_username = cmd->args[0];
        table* bios_table = get_table(cmdGlobals.db, "bio", 0);
        if (bios_table == NULL) {
            printf("Bios table not found in database.\n");
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Bios table not found.");
            return res;
        }
        char* bio = get(bios_table, target_username);
        if (bio == NULL) {
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "No bio found for user %s.", target_username);
            return res;
        }
        res->status_code = 1; // Success
        res->message_size = snprintf(res->message, MAX_ARG_LEN, "%s", bio);
        return res;
    } else if (strcmp(cmd->command, "WRITE_BIO") == 0) {
        if (cmd->args_size != 1) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
        const char* new_bio = cmd->args[0];
        char* username = get(cmdGlobals.user_fds, fd_string);
        if (username == NULL) {
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "No registered username for fd %d.\n", client_socket_fd);
            return res;
        }
        table* bios_table = get_table(cmdGlobals.db, "bios", 0);
        if (bios_table == NULL) {
            printf("Bios table not found in database.\n");
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Bios table not found.");
            return res;
        }
        if (!insert(bios_table, username, new_bio)) {
            printf("Error updating bio in database.\n");
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Couldn't update bio in database.");
            return res;
        }
        printf("User %s updated their bio.\n", username);
        res->status_code = 1; // Success
        return res;
    } else if (strcmp(cmd->command, "RETRIEVE_GAMES") == 0) {
        if (cmd->args_size != 0) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
        // Retrieve list of active games 
        res->status_code = 1; // Success
        if (cmdGlobals.running_games_count == 0)
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "No running games at the moment.");

        for (size_t i = 0; i < cmdGlobals.running_games_count; i++) {  // Copy player names into response body
            char game_info[MAX_ARG_LEN];
            snprintf(game_info, MAX_ARG_LEN, "%d,%s,%s", 
                     cmdGlobals.running_games[i]->game_id,
                     cmdGlobals.running_games[i]->playernames[0],
                     cmdGlobals.running_games[i]->playernames[1]);
            strcpy(res->body[res->body_size++], game_info);
        }
        return res;
    } else if (strcmp(cmd->command, "SPECTATE") == 0) {
        if (cmd->args_size != 1) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
        // Spectate a game by ID
        int game_id = atoi(cmd->args[0]);
        Awale* target_game = NULL;
        for (size_t i = 0; i < cmdGlobals.running_games_count; i++) {
            if (cmdGlobals.running_games[i]->game_id == game_id) {
                target_game = cmdGlobals.running_games[i];
                break;
            }
        }
        if (target_game == NULL) {
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Game ID %d not found.", game_id);
            return res;
        }
        // Add spectator's fd to the game's spectator list
        target_game->spectators_fds = realloc(target_game->spectators_fds,
                                             sizeof(int) * (target_game->spectators_count + 1));
        if (target_game->spectators_fds == NULL) {
            printf("Error allocating memory for spectator fds.\n");
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Couldn't allocate memory for spectator.");
            return res;
        }
        target_game->spectators_fds[target_game->spectators_count++] = client_socket_fd;
        res->status_code = 1; // Success
        return res;
    } else if (strcmp(cmd->command, "RETRIEVE_SELF_GAMES") == 0) {
        if (cmd->args_size != 0) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }

        char* username = get(cmdGlobals.user_fds, fd_string);
        if (username == NULL) {
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "No registered username for fd %d.\n", client_socket_fd);
            return res;
        }
        // Retrieve past games involving this user from the database
        table* games_table = get_table(cmdGlobals.db, "user-games", 0);
        if (games_table == NULL) {
            printf("User-games table not found in database.\n");
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "User-games table not found.");
            return res;
        }

        res->status_code = 1; // Success
        char* game_ids_list = get(games_table, username);
        if (game_ids_list == NULL) {
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "No past games found for user %s.", username);
            return res;
        }
        // Split game_ids_list by commas and add to response body
        for (int i = 0; i < MAX_VALUE_LEN; i++) {
            char currGameID[MAX_ARG_LEN];
            int currIndex = 0;
            while (game_ids_list[i] != ',' && game_ids_list[i] != '\0') {
                currGameID[currIndex++] = game_ids_list[i];
                i++;
            }
            currGameID[currIndex] = '\0';
            strcpy(res->body[res->body_size++], currGameID);
            if (game_ids_list[i] == '\0')
                break;
        }
        return res;
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
        // Retrieve list of chats involving the user
        char* username = get(cmdGlobals.user_fds, fd_string);
        if (username == NULL) {
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "No registered username for fd %d.\n", client_socket_fd);
            return res;
        }
        table* chats_table = get_table(cmdGlobals.db, "chats", 0);
        if (chats_table == NULL) {
            printf("Chats table not found in database.\n");
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Chats table not found.");
            return res;
        }
        res->status_code = 1; // Success
        // For each entry in chats_table, check if username is part of the key
        for (int i = 0; i < chats_table->size; i++) {
            char* chat_key = chats_table->entries[i]->key;
            if (strstr(chat_key, username) != NULL) {
                // Extract the other username from the key
                char other_username[MAX_ARG_LEN];
                sscanf(chat_key, "%[^_]_%s", other_username, other_username + strlen(other_username) + 1);
                if (strcmp(other_username, username) == 0) // Avoid adding self
                    strcpy(other_username, chat_key + strlen(username) + 1); // Skip underscore
                strcpy(res->body[res->body_size++], other_username);
            }
        }

        if (res->body_size == 0) {
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "No chats found for user %s.", username);
        }
        return res;
    } else if (strcmp(cmd->command, "OPEN_CHAT") == 0) {
        if (cmd->args_size != 1) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
        // Retrieve chat history with specified user
        const char* chat_key = cmd->args[0];
        table* chats_table = get_table(cmdGlobals.db, "chats", 0);
        if (chats_table == NULL) {
            printf("Chats table not found in database.\n");
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Chats table not found.");
            return res;
        }
        char* chat_history = get(chats_table, chat_key);
        if (chat_history == NULL) {
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "No chat history found for chat %s.", chat_key);
            return res;
        }
        // Split chat_history by 'username-message|username-message|...'
        for (int i = 0; i < MAX_VALUE_LEN; i++) {
            // Get user who sent the message
            char sender_username[MAX_ARG_LEN];
            int sender_index = 0;
            while (chat_history[i] != '-' && chat_history[i] != '\0') {
                sender_username[sender_index++] = chat_history[i];
                i++;
            }
            sender_username[sender_index] = '\0';
            if (chat_history[i] == '\0')
                break;
            i++; // Skip '-'
            
            // Get the message content
            char message_content[MAX_ARG_LEN];
            int message_index = 0;
            while (chat_history[i] != '|' && chat_history[i] != '\0') {
                message_content[message_index++] = chat_history[i];
                i++;
            }
            message_content[message_index] = '\0';
            // Format: "sender,message"
            char formatted_message[MAX_ARG_LEN];
            snprintf(formatted_message, MAX_ARG_LEN, "%s,%s", sender_username, message_content);
            strcpy(res->body[res->body_size++], formatted_message);
            if (chat_history[i] == '\0')
                break;
            i++; // Skip '|'
        }
        res->status_code = 1; // Success
        return res;
    } else if (strcmp(cmd->command, "SEND_MSG") == 0) {
        if (cmd->args_size != 2) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
        // Add message to chat history
        const char* chat_key = cmd->args[0];
        const char* message_content = cmd->args[1];
        char* sender_username = get(cmdGlobals.user_fds, fd_string);
        if (sender_username == NULL) {
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "No registered username for fd %d.\n", client_socket_fd);
            return res;
        }
        table* chats_table = get_table(cmdGlobals.db, "chats", 0);
        if (chats_table == NULL) {
            printf("Chats table not found in database.\n");
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Chats table not found.");
            return res;
        }
        char* existing_chat_history = get(chats_table, chat_key);
        char new_chat_entry[MAX_VALUE_LEN];
        if (existing_chat_history == NULL)
            snprintf(new_chat_entry, MAX_VALUE_LEN, "%s-%s", sender_username, message_content);
        else
            snprintf(new_chat_entry, MAX_VALUE_LEN, "%s|%s-%s", existing_chat_history, sender_username, message_content);
        
        if (!insert(chats_table, chat_key, new_chat_entry)) {
            printf("Error inserting message into chats table.\n");
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Couldn't insert message into database.");
            return res;
        }
        res->status_code = 1; // Success
        return res;
    } else if (strcmp(cmd->command, "CREATE_CHAT") == 0) {
        if (cmd->args_size != 1) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
        const char* target_username = cmd->args[0];
        char* username = get(cmdGlobals.user_fds, fd_string);
        if (username == NULL) {
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "No registered username for fd %d.\n", client_socket_fd);
            return res;
        }
        // Create chat key in the form username1_username2 (sorted lexicographically)
        char chat_key[MAX_ARG_LEN];
        if (strcmp(username, target_username) < 0)
            snprintf(chat_key, MAX_ARG_LEN, "%s_%s", username, target_username);
        else
            snprintf(chat_key, MAX_ARG_LEN, "%s_%s", target_username, username);
        table* chats_table = get_table(cmdGlobals.db, "chats", 0);
        if (chats_table == NULL) {
            printf("Chats table not found in database.\n");
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Chats table not found.");
            return res;
        }
        // Initialize empty chat history
        if (!insert(chats_table, chat_key, "")) {
            printf("Error creating new chat in chats table.\n");
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Couldn't create new chat in database.");
            return res;
        }
        res->status_code = 1; // Success
        return res;
    } else if (strcmp(cmd->command, "RETRIEVE_FRIENDS") == 0) {
        if (cmd->args_size != 0) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
        // Retrieve list of friends for the user
        char* username = get(cmdGlobals.user_fds, fd_string);
        if (username == NULL) {
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "No registered username for fd %d.\n", client_socket_fd);
            return res;
        }
        table* friends_table = get_table(cmdGlobals.db, "friends", 0);
        if (friends_table == NULL) {
            printf("Friends table not found in database.\n");
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Friends table not found.");
            return res;
        }
        char* friends_list = get(friends_table, username);
        if (friends_list == NULL) {
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "No friends found for user %s.", username);
            return res;
        }
        // Split friends_list by commas and add to response body
        for (int i = 0; i < MAX_VALUE_LEN; i++) {
            char currFriend[MAX_ARG_LEN];
            int currIndex = 0;
            while (friends_list[i] != ',' && friends_list[i] != '\0') {
                currFriend[currIndex++] = friends_list[i];
                i++;
            }
            currFriend[currIndex] = '\0';
            strcpy(res->body[res->body_size++], currFriend);
            if (friends_list[i] == '\0')
                break;
        }
        res->status_code = 1; // Success
        return res;
    } else if (strcmp(cmd->command, "ADD_FRIEND") == 0) {
        if (cmd->args_size != 1) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
        // Add a friend to the user's friend list
        const char* new_friend_username = cmd->args[0];
        char* username = get(cmdGlobals.user_fds, fd_string);
        if (username == NULL) {
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "No registered username for fd %d.\n", client_socket_fd);
            return res;
        }
        table* friends_table = get_table(cmdGlobals.db, "friends", 0);
        if (friends_table == NULL) {
            printf("Friends table not found in database.\n");
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Friends table not found.");
            return res;
        }
        char* existing_friends = get(friends_table, username);
        char new_friends_entry[MAX_VALUE_LEN];
        if (existing_friends == NULL)
            snprintf(new_friends_entry, MAX_VALUE_LEN, "%s", new_friend_username);
        else
            snprintf(new_friends_entry, MAX_VALUE_LEN, "%s,%s", existing_friends, new_friend_username);
        // Update the friends table with the new friends list
        if (!insert(friends_table, username, new_friends_entry)) {
            printf("Error adding friend to friends table.\n");
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Couldn't add friend to database.");
            return res;
        }
        res->status_code = 1; // Success
        return res;
    } else if (strcmp(cmd->command, "REMOVE_FRIEND") == 0) {
        if (cmd->args_size != 1) {
            printf("Invalid number of arguments for %s command.\n", cmd->command);
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Invalid number of arguments for %s.", cmd->command);
            return res;
        }
        // Remove a friend from the user's friend list
        const char* remove_friend_username = cmd->args[0];
        char* username = get(cmdGlobals.user_fds, fd_string);
        if (username == NULL) {
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "No registered username for fd %d.\n", client_socket_fd);
            return res;
        }
        table* friends_table = get_table(cmdGlobals.db, "friends", 0);
        if (friends_table == NULL) {
            printf("Friends table not found in database.\n");
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Friends table not found.");
            return res;
        }
        char* existing_friends = get(friends_table, username);
        if (existing_friends == NULL) {
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "No friends found for user %s.", username);
            return res;
        }
        // Remove the specified friend from the existing friends list
        char new_friends_entry[MAX_VALUE_LEN] = "";
        int first = 1;
        for (int i = 0; i < MAX_VALUE_LEN; ) {
            char currFriend[MAX_ARG_LEN];
            int currIndex = 0;
            while (existing_friends[i] != ',' && existing_friends[i] != '\0') {
                currFriend[currIndex++] = existing_friends[i];
                i++;
            }
            currFriend[currIndex] = '\0';
            if (strcmp(currFriend, remove_friend_username) != 0) {
                if (!first) {
                    strncat(new_friends_entry, ",", MAX_VALUE_LEN - strlen(new_friends_entry) - 1);
                }
                strncat(new_friends_entry, currFriend, MAX_VALUE_LEN - strlen(new_friends_entry) - 1);
                first = 0;
            }
            if (existing_friends[i] == '\0')
                break;
            i++; // Skip comma
        }
        // Update the friends table with the new friends list
        if (!insert(friends_table, username, new_friends_entry)) {
            printf("Error removing friend from friends table.\n");
            res->message_size = snprintf(res->message, MAX_ARG_LEN, "Couldn't remove friend from database.");
            return res;
        }
        res->status_code = 1; // Success
        return res;
    } else {
        printf("Unknown command: %s\n", cmd->command);
        res->message_size = snprintf(res->message, MAX_ARG_LEN, "Unknown command: %s", cmd->command);
        return res;
    }
    return res;
}
