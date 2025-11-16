#ifndef _DATABASE_H_
#define _DATABASE_H_

#include "common.h"
#include "kvs.h"

#define DB_SCHEMA_SIZE 5

extern const char* DB_SCHEMA[DB_SCHEMA_SIZE];
extern pthread_mutex_t mut_database;

/**
 * \brief This struct defines the interface for CRUD operations in the database. It also handles the creation of tables.
 */
typedef struct database {
	table** tables;
	int size;
} database;

database* create_database();
int apply_database_schema(database* d, const char** schema, int schema_size, int locked);
int delete_database(database* d, int locked);
table* add_table(database* d, const char* name, int locked);
const table* get_table(const database* d, const char* name, int locked);
int validate_database(const database* d, int locked);
int save_database(const database* d, const char* filename, int locked);
database* load_database(const char* filename);

#endif // _DATABASE_H_
