#include "server.h"

#include "database.h"

int main() {
	database* myDB = load_database("database.db");

	table* tab = myDB->tables[0];

	printf("Database size: %d\nTable name: %s\nEntry: %s - %s\n", myDB->size, tab->name, tab->entries[0]->key, tab->entries[0]->value);

	delete_database(myDB);
	return 0;
}
