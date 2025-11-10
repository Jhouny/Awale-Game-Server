#include "database.h"

database* create_database() {
	database* d = (database*) malloc(sizeof(database));
	if (d == NULL)
		return NULL;
	d->size = 0;
	d->tables = NULL;
	return d;
}

int delete_database(database* d) {
	if (d == NULL)
		return 0;
		
	for (int i = 0; i < d->size; i++) {
		delete_table(d->tables[i]);
	}
	free(d->tables);
	free(d);
	return 1;
}

table* add_table(database* d, const char* name) {
	if (d == NULL || name == NULL)
		return NULL;
	
	d->size += 1;
	d->tables = (table**) realloc(d->tables, sizeof(table*) * d->size);
	if (d->tables == NULL)
		return NULL;
	d->tables[d->size - 1] = create_table();
	if (d->tables[d->size - 1] == NULL)
		return NULL;
	set_table_name(d->tables[d->size - 1], name);
	return d->tables[d->size - 1];
}

const table* get_table(const database* d, const char* name) {
	if (d == NULL || name == NULL)
		return NULL;

	for (int i = 0; i < d->size; i++) {
		if (strcmp(d->tables[i]->name, name) == 0) {
			return d->tables[i];
		}
	}
	return NULL;
}




