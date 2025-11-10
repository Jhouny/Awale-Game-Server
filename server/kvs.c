#include "kvs.h"

entry* create_entry() {
	entry* e = (entry*) malloc(sizeof(entry));
	e->free = 1;
	return e;
}

table* create_table() {
	table* t = (table*) malloc(sizeof(table));
	t->size = 20;
	t->entries = (entry**) malloc(t->size * sizeof(entry*));
	for (int i = 0; i < t->size; i++) {
		t->entries[i] = create_entry();
	}
	t->name[0] = '\0';
	return t;
}

void set_table_name(table* t, const char* name) {
	memcpy(t->name, name, 32);
	t->name[31] = '\0';
}

int insert(table* t, const char* key, const char* value) {
	// Check if key already exists in table. If not, add it.
	for (int i = 0; i < t->size; i++) {
		entry* curr = t->entries[i];
		if (!curr->free && strcmp(curr->key, key) == 0)
			return 0;
	}
	
	int found_index = -1;
	for (int i = 0; i < t->size; i++) {
		if (t->entries[i]->free) {
			found_index = i;
			break;
		}
	}

	if (found_index == -1) { // If no free pre-allocated entry, reallocate more memory
		int prevSize = t->size;
		t->size = t->size * 2;  // Pre-allocate twice the previous size for a good margin
		t->entries = (entry**) realloc(t->entries, sizeof(entry*) * t->size);
		for (int i = prevSize; i < t->size; i++) {
			t->entries[i] = create_entry();
		}
		found_index = prevSize;
	}
	
	// Insert new entry
	strcpy(t->entries[found_index]->key, key);
	strcpy(t->entries[found_index]->value, value);
	t->entries[found_index]->free = 0;

	return 1;
}

int insert_entry(table* t, const entry* e) {
	char key[MAX_KEY_LEN];
	char value[MAX_VALUE_LEN];
	strcpy(key, e->key);
	strcpy(value, e->value);
	return insert(t, key, value);
}

char* get(const table* t, const char* key) {
	for (int i = 0; i < t->size; i++) {
		if (!t->entries[i]->free && strcmp(t->entries[i]->key, key) == 0)
			return t->entries[i]->value;
	}
	return '\0';
}
