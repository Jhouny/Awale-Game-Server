#include "server.h"

#include "database.h"

int main() {
	table myTable;
	set_table_name(&myTable, "this is my table");
	printf("Table name: %s\n", myTable.name);
}
