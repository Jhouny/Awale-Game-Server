#include "database.h"

static const char DB_SCHEMA[][32] = {
	"users",
	"bio",
	"user-games",
	"chats",		// Stores chats in the form: username1_username2 -> message1|message2|...
	"friends",
};

database* create_database() {
	database* d = (database*) malloc(sizeof(database));
	if (d == NULL)
		return NULL;
	d->size = 0;
	d->tables = NULL;
	return d;
}

int apply_database_schema(database* d) {
	pthread_mutex_lock(&mut_database);
	if (d == NULL) {
		pthread_mutex_unlock(&mut_database);
		return 0;
	}

	for (int i = 0; i < DB_SCHEMA_SIZE; i++) {
		if (get_table(d, DB_SCHEMA[i], 1) == NULL) {
			add_table(d, DB_SCHEMA[i], 1);
		}
	}
	pthread_mutex_unlock(&mut_database);
	return 1;
}

int delete_database(database* d) {
	pthread_mutex_destroy(&mut_database);
	if (d == NULL)
		return 0;

	for (int i = 0; i < d->size; i++) {
		delete_table(d->tables[i], 1);
	}
	free(d->tables);
	free(d);
	return 1;
}

table* add_table(database* d, const char* name, int locked) {
	if (!locked)
		pthread_mutex_lock(&mut_database);
	if (d == NULL || name == NULL) {
		if (!locked)
			pthread_mutex_unlock(&mut_database);
		return NULL;
	}
	
	d->size += 1;
	d->tables = (table**) realloc(d->tables, sizeof(table*) * d->size);
	if (d->tables == NULL) {
		if (!locked)
			pthread_mutex_unlock(&mut_database);
		return NULL;
	}
	d->tables[d->size - 1] = create_table();
	if (d->tables[d->size - 1] == NULL) {
		if (!locked)
			pthread_mutex_unlock(&mut_database);
		return NULL;
	}
	set_table_name(d->tables[d->size - 1], name, 1);
	if (!locked)
		pthread_mutex_unlock(&mut_database);
	return d->tables[d->size - 1];
}

const table* get_table(const database* d, const char* name, int locked) {
	if (!locked)
		pthread_mutex_lock(&mut_database);
	if (d == NULL || name == NULL) {
		if (!locked)
			pthread_mutex_unlock(&mut_database);
		return NULL;
	}

	for (int i = 0; i < d->size; i++) {
		if (strcmp(d->tables[i]->name, name) == 0) {
			if (!locked)
				pthread_mutex_unlock(&mut_database);
			return d->tables[i];
		}
	}
	if (!locked)
		pthread_mutex_unlock(&mut_database);
	return NULL;
}

int validate_database(const database* d) {
	pthread_mutex_lock(&mut_database);
	if (d == NULL) {
		pthread_mutex_unlock(&mut_database);
		return 0;
	}

	for (int i = 0; i < DB_SCHEMA_SIZE; i++) {
		if (get_table(d, DB_SCHEMA[i], 1) == NULL) {
			pthread_mutex_unlock(&mut_database);
			return 0;
		}
	}
	pthread_mutex_unlock(&mut_database);
	return 1;
}

int save_database(const database* d, const char* filename) {
	pthread_mutex_lock(&mut_database);
	if (d == NULL || filename == NULL) {
		pthread_mutex_unlock(&mut_database);
		return 0;
	}

	FILE* file = fopen(filename, "wb");
	if (file == NULL)
		return 0;

	// Saving goes here
	// Save database size info
	fwrite(&d->size, sizeof(int), 1, file);
	// Save tables
	for (int i = 0; i < d->size; i++) {  // For each table
		fwrite(&d->tables[i]->size, sizeof(int), 1, file);
		fwrite(d->tables[i]->name, sizeof(char), 32, file);
		for (int j = 0; j < d->tables[i]->size; j++) {  // For each entry
			fwrite(d->tables[i]->entries[j]->key, sizeof(char), MAX_KEY_LEN, file);
			fwrite(d->tables[i]->entries[j]->value, sizeof(char), MAX_VALUE_LEN, file);
		}
	}

	fclose(file);
	pthread_mutex_unlock(&mut_database);
	return 1;
}

database* load_database(const char* filename) {
	if (filename == NULL)
		return NULL;

	FILE* file = fopen(filename, "rb");
	if (file == NULL)
		return NULL;

	database* d = create_database();
	if (d == NULL) {
		fclose(file);
		return NULL;
	}

	// Loading goes here
	// Read db size
	int db_size;
	fread(&db_size, sizeof(int), 1, file);
	// Read tables
	for (int i = 0; i < db_size; i++) {  // For each table
		char table_name[32];
		int table_size;
		fread(&table_size, sizeof(int), 1, file);
		fread(table_name, sizeof(char), 32, file);
		add_table(d, table_name, 0);
		for (int j = 0; j < table_size; j++) {  // For each entry	
			char entry_key[MAX_KEY_LEN];
			char entry_value[MAX_VALUE_LEN];
			fread(entry_key, sizeof(char), MAX_KEY_LEN, file);
			fread(entry_value, sizeof(char), MAX_VALUE_LEN, file);
			insert(d->tables[i], entry_key, entry_value);
		}
	}

	fclose(file);
	return d;
}

