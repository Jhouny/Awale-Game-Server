#include "server.h"

#include "database.h"

int main() {
	database* myDB = create_database();

	table* tab = add_table(myDB, "my-first-tablee");

	insert(tab, "myKey", "myValue");
	insert(tab, "random-key", "random-value");

	table* tb2 = add_table(myDB, "second Table in the database");
	insert(tb2, "a Key", "one value");

	printf("Database size: %d\nTable name: %s\nEntry: %s - %s\n", myDB->size, tab->name, tab->entries[0]->key, tab->entries[0]->value);

	delete_database(myDB);
	return 0;
}
