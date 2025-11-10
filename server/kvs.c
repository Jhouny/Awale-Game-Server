#include "kvs.h"

entry* create_entry() {
	entry* e = (entry*) malloc(sizeof(entry));
	e->free = 1;
	return e;
}

int delete_entry(entry* e) {
	if (e == NULL)
		return 0;
	free(e);
	return 1;
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

int delete_table(table* t) {
	if (t == NULL)
		return 0;
	for (int i = 0; i < t->size; i++) {
		delete_entry(t->entries[i]);
	}
	free(t->entries);
	free(t);
	return 1;
}

void set_table_name(table* t, const char* name) {
	strncpy(t->name, name, 31);
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
	strncpy(t->entries[found_index]->key, key, MAX_KEY_LEN - 1);
	t->entries[found_index]->key[MAX_KEY_LEN - 1] = '\0';
	strncpy(t->entries[found_index]->value, value, MAX_VALUE_LEN - 1);
	t->entries[found_index]->value[MAX_VALUE_LEN - 1] = '\0';
	t->entries[found_index]->free = 0;

	return 1;
}

int insert_entry(table* t, const entry* e) {
	return insert(t, e->key, e->value);
}

int remove_entry(table* t, const char* key) {
	for (int i = 0; i < t->size; i++) {
		if (!t->entries[i]->free && strcmp(t->entries[i]->key, key) == 0) {
			t->entries[i]->free = 1; // Mark as free
			return 1;
		}
	}
	return 0; // Not found
}

char* get(const table* t, const char* key) {
	for (int i = 0; i < t->size; i++) {
		if (!t->entries[i]->free && strcmp(t->entries[i]->key, key) == 0)
			return t->entries[i]->value;
	}
	return NULL;
}