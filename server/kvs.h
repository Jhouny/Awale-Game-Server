#ifndef _KVS_H_
#define _KVS_H_

#include "common.h"

/**
 * \brief This struct represents an entry on the KVS. It is a key-value unit.
 */
typedef struct kv {
	char*   key;
	char* value;
	char   free;  // Used to pre-allocate memory for speeding up the program. '1' means it's free, '0' means it's been allocated.
} entry;

/**
 * \brief This struct defines a Key-Value storage unit. It corresponds to a table on the database. 
 */
typedef struct kvs {
	char name[32]; // Maximum name length = 32 chars
	entry* entries;	
	int size;
} table;

void initialize_table(table* t);
void set_table_name(table* t, const char* name);
int insert(table* t, const char* key, const char* value);
int insert_entry(table* t, const entry e);
char* get(const table* t, const char* key);

#endif
