#include "server.h"

#include "database.h"
#include "Network.h"
#include "commander.h"

int main(int argc, char* argv[]) {
	if (!parse_args(argc, argv))
		return 1;

	if (pthread_mutex_init(&mut_database, NULL) != 0) {
		printf("Error initializing database mutex.\n");
		return 1;
	}
	database* db = load_database(globals.db_file);

	if (!validate_database(db)) {
		printf("Database at %s is invalid or corrupted. Creating a new one...\n", globals.db_file);
		if (db != NULL)
			delete_database(db);
		db = NULL;
	}

	if (db == NULL) {
		printf("Couldn't open database at %s, creating an empty one...\n", globals.db_file);
		db = create_database();
		apply_database_schema(db);
	} else {
		printf("Loaded database from %s.\n\tBacking it up to %s.\n", globals.db_file, DATABASE_BACKUP_FILE);
		save_database(db, DATABASE_BACKUP_FILE);
	}

	// Add database to commander globals
	cmdGlobals.db = db;

	// Create connected users to file descriptor association table
	table* user_fds = create_table();
	set_table_name(user_fds, "user_fds", 1);
	cmdGlobals.user_fds = user_fds;
	
	// Start server
	printf("\nStarting database server on port %s.\n", globals.port);
	int socket_fd = setup_server();
	if (socket_fd != -1) {
		if (pthread_mutex_init(&mut_client_sockets_fd, NULL) != 0) {
			printf("Error initializing client sockets mutex.\n");
			return 1;
		}
		pthread_t connectThread, dbSaveThread;
		int* param = (int *) malloc(sizeof(int));
		param[0] = socket_fd;
		
		// Start thread to accept new connections
		if (pthread_create(&connectThread, NULL, connection_loop, param) != 0) {
			printf("Error creating socket thread.\n");
			return 1;
		}
		
		// Prepare parameters for database save thread
		database** db_param = (database**) malloc(sizeof(database*));
		db_param[0] = db;
		// Start thread to periodically save database to disk
		if (pthread_create(&dbSaveThread, NULL, database_save_loop, db_param) != 0) {
			printf("Error creating database thread.\n");
			return 1;
		}
	
		// Listen to received commands from client and respond accordingly
		while (1) {
			// Poll for new command from client
			// Print mutex state for debugging

			pthread_mutex_lock(&mut_client_sockets_fd);
			int events = poll(client_sockets_fd, total_clients, 0);
			if (events == 0) {
				pthread_mutex_unlock(&mut_client_sockets_fd);
				continue;  // No data received, continue polling
			} else if (events == -1) {
				printf("Error polling sockets.\n");
				pthread_mutex_unlock(&mut_client_sockets_fd);
				return 1;
			}

			for (int i = 0; i < (int)total_clients; i++) {
				if (client_sockets_fd[i].revents & POLLIN) {  // Data received and available
					Command* cmd = receive_and_deserialize_Command(client_sockets_fd[i].fd);
					if (cmd == NULL) {
						printf("Error receiving command from client. Closing connection.\n");
						fflush(stdout);
						pthread_mutex_unlock(&mut_client_sockets_fd);
						remove_client(client_sockets_fd[i].fd);
						break;
					}
						
					printf("Received command '%s' from client socket %d.\n", cmd->command, client_sockets_fd[i].fd);
					// Handle command
					Response* res = execute_command(cmd, client_sockets_fd[i].fd);
					if (res == NULL) {
						printf("Error executing command from client.\n");
						fflush(stdout);
						pthread_mutex_unlock(&mut_client_sockets_fd);
						break;
					}

					// Log response
					printf("Sending response with status code %d and body size %d to client socket %d.\n", res->status_code,  res->body_size, client_sockets_fd[i].fd);
					fflush(stdout);
					for (int j = 0; j < res->body_size; j++) {
						printf("  - Body[%d]: %s\n", j, res->body[j]);
					}
					fflush(stdout);

					// Send response back to client
					if (serialize_and_send_Response(client_sockets_fd[i].fd, res) != 0) {
						printf("Error sending response to client. Closing connection.\n");
						fflush(stdout);
						pthread_mutex_unlock(&mut_client_sockets_fd);
						remove_client(client_sockets_fd[i].fd);
						break;
					}

					// Free command after processing
					for (int i = 0; i < cmd->args_size; i++) {
						free(cmd->args[i]);
					}
					free(cmd);
				} else if (client_sockets_fd[i].revents & POLLHUP) {  // Peer connection closed
					printf("Client connection closed at socket %d.\n", client_sockets_fd[i].fd);
					pthread_mutex_unlock(&mut_client_sockets_fd);
					remove_client(client_sockets_fd[i].fd);
					break;
				} else if (client_sockets_fd[i].revents & POLLNVAL) {  // Invalid request
					printf("Client connection closed at socket %d.\n", client_sockets_fd[i].fd);
					pthread_mutex_unlock(&mut_client_sockets_fd);
					remove_client(client_sockets_fd[i].fd);
					break;
				}
			}
			pthread_mutex_unlock(&mut_client_sockets_fd);
		}
		pthread_join(connectThread, NULL);
		pthread_join(dbSaveThread, NULL);
	}

	pthread_mutex_destroy(&mut_client_sockets_fd);
	// Free allocated database from memory
	delete_database(db);
	return 0;
}


int parse_args(int argc, char* argv[]) {
	strncpy(globals.db_file, DATABASE_FILE, 4096);
	globals.db_file[4096] = '\0';
	strncpy(globals.port, DEFAULT_PORT, 5);
	globals.port[5] = '\0';

	for (int i = 1; i < argc-1; i += 2) {
		char* arg = strstr(argv[i], "--");
		arg = &arg[2];
		if (strstr(argv[i+1], "--") != NULL) {  // No value attributed 
			printf("No value given for argument '%s'.\n", arg);
			return 0;
		}
		
		if (strcmp(arg, "port") == 0) {
			strncpy(globals.port, argv[i+1], 5);
			globals.port[5] = '\0';
		} else if (strcmp(arg, "db") == 0 || strcmp(arg, "database") == 0) {
			strncpy(globals.db_file, argv[i+1], 4096);
			globals.db_file[4096] = '\0';
		} else {
			printf("Argument invalid.\n");
			return 0;
		}
	}
	return 1;
}

int setup_server() {
	int socket_fd;
	struct sockaddr_in server_addr;

	// Open socket 
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1) {
		printf("Error opening socket.\n");
		return -1;
	}

	// Zero-fill server_addr and apply properties
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(globals.port));
	server_addr.sin_addr.s_addr = INADDR_ANY;

	// Bind socket to port and address. Must cast to expected type sockaddr_in --> sockaddr.
	if (bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
		printf("Error binding socket.\n");
		return -1;
	}

	// Setup socket to listen (accept connections). Also set queue length of established sockets waiting to connect to 100.
	if (listen(socket_fd, 100) == -1) {
		printf("Error setting up socket to listen.\n");
		return -1;
	}

	return socket_fd;
}

int add_client(int new_client_sfd) {
	// Print mutex state for debugging
	pthread_mutex_lock(&mut_client_sockets_fd);
    size_t new_total = total_clients + 1;
    struct pollfd* tmp = (struct pollfd*) realloc(client_sockets_fd, new_total * sizeof(struct pollfd));
    if (tmp == NULL) {
        pthread_mutex_unlock(&mut_client_sockets_fd);
        return 1;
    }
    client_sockets_fd = tmp;
    client_sockets_fd[new_total - 1].fd = new_client_sfd;
    client_sockets_fd[new_total - 1].events = POLLIN;
    total_clients = new_total;
    pthread_mutex_unlock(&mut_client_sockets_fd);
    return 0;
}

int remove_client(int client_sfd) {
	// Print mutex state for debugging
	pthread_mutex_lock(&mut_client_sockets_fd);
    int found = 0;
    for (int i = 0; i < (int)total_clients; i++) {
        if (client_sockets_fd[i].fd == client_sfd) {
            found = 1;
            // Shift all subsequent elements left
            for (int j = i; j < (int)total_clients - 1; j++) {
                client_sockets_fd[j] = client_sockets_fd[j + 1];
            }
            break;
        }
    }
    if (found) {
        total_clients -= 1;
        if (total_clients > 0) {
            client_sockets_fd = realloc(client_sockets_fd, total_clients * sizeof(struct pollfd));
        } else {
            free(client_sockets_fd);
            client_sockets_fd = NULL;
        }
    }
    pthread_mutex_unlock(&mut_client_sockets_fd);
	if (client_sfd > 2)
		close(client_sfd);
    return 0;
}

void *connection_loop(void *param) {
    int socket_fd = ((int *)param)[0];

    struct sockaddr_storage client_addr;
    socklen_t client_len = sizeof(client_addr);

	struct pollfd server_socket_poll[1];
	server_socket_poll[0].fd = socket_fd;
	server_socket_poll[0].events = POLLIN;

    while (1) {
		int events = poll(server_socket_poll, 1, 1000);
		if (events == 0)
			continue;  // No data received, continue polling
		else if (events == -1) {
			printf("Error polling sockets.\n");
            return (void*)(intptr_t) 1;
		}

        int client_socket_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket_fd == -1) {
            perror("accept");
            printf("Error accepting new connection.\n");
            return (void*)(intptr_t) 1;
        }
        printf("New connection accepted. fd=%d\n", client_socket_fd);

        if (add_client(client_socket_fd) == 1) {
            printf("Error adding client to sockets list.\n");
            return (void*)(intptr_t) 1;
        }
    }
}

void *database_save_loop(void *param) {
	database* db = ((database **)param)[0];

	while (1) {
		sleep(DATABASE_SAVE_INTERVAL);
		printf("Saving database to disk...\n");
		if (save_database(db, globals.db_file) != 1) {
			printf("Error saving database to disk.\n");
		} else {
			printf("Database saved successfully.\n");
		}
	}
}
