#ifndef _DATABASE_H_
#define _DATABASE_H_

#include "kvs.h"

/**
 * \brief This struct defines the interface for CRUD operations in the database. It also handles the creation of tables.
 */
typedef struct database {
	table** tables;
	int size;
} database;

database* create_database();
table* add_table(database* d, const char* name);
const table* get_table(const database* d, const char* name);

#endif // _DATABASE_H_
