#include "server.h"

#include "database.h"

int main(int argc, char* argv[]) {
	if (!parse_args(argc, argv))
		return 1;

	database* db = load_database(globals.db_file);

	if (db == NULL) {
		printf("Couldn't open database at %s, creating an empty one...\n", globals.db_file);
		db = create_database();
	} else {
		printf("Loaded database from %s.\n\tBacking it up to %s.\n", globals.db_file, DATABASE_BACKUP_FILE);
		save_database(db, DATABASE_BACKUP_FILE);
	}

	// Start server
	printf("\nStarting server on port %s.\n", globals.port);
	int socket_fd = setup_server();
	if (socket_fd != -1) {
		struct sockaddr* client_addr;	
		socklen_t client_len = sizeof(client_addr);
		while (1) {  // Server runs indefinitely
			int client_socket_fd = accept(socket_fd, client_addr, &client_len);
			if (client_socket_fd == -1) {
				printf("Error accepting new connection.\n");
				return 1;
			}
			printf("New connection accepted.\n");
			
			// Handle new connection here
		}
	}

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
	socket_fd = socket(AF_INET, SOCK_STREAM, 0)
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
