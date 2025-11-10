#include "database.h"

database* create_database() {
	database* d = (database*) malloc(sizeof(database));
	d->size = 0;
	d->tables = NULL;
	return d;
}

table* add_table(database* d, const char* name) {
	d->size += 1;
	d->tables = (table**) realloc(d->tables, sizeof(table*) * d->size);
	d->tables[d->size - 1] = create_table();
	set_table_name(d->tables[d->size - 1], name);
	return d->tables[d->size - 1];
}

const table* get_table(const database* d, const char* name) {
	for (int i = 0; i < d->size; i++) {
		if (strcmp(d->tables[i]->name, name) == 0) {
			return d->tables[i];
		}
	}
	return NULL;
}




