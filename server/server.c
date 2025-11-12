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
