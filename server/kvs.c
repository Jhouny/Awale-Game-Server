#include "kvs.h"

void initialize_table(table* t) {
	t->size = 20;
	t->entries = (entry*) malloc(t->size * sizeof(entry));
	for (int i = 0; i < t->size; i++) {
		t->entries[i].free = 1;
	}
	t->name[0] = '\0';
}

void set_table_name(table* t, const char* name) {
	memcpy(t->name, name, 32);
	t->name[31] = '\0';
}

int insert(table* t, const char* key, const char* value) {
	// Check if key already exists in table. If not, add it.
	for (int i = 0; i < t->size; i++) {
		entry curr = t->entries[i];
		if (!curr.free && strcmp(curr.key, key) == 0)
			return 0;
	}
	
	int i = 0;
	int found_ = 0;
	for (; i < t->size && found_ == 0; i++) {
		if (t->entries[i].free)
			found_ = 1;
	}

	if (!found_) { // If no free pre-allocated entry, reallocate more memory
		t->size = t->size * 2;  // Pre-allocate twice the previous size for a good margin
		t->entries = (entry*) realloc(t->entries, sizeof(entry) * t->size);
	}
	
	// Insert new entry
	strcpy(t->entries[i].key, key);
	strcpy(t->entries[i].value, value);

	return 1;
}

int insert_entry(table* t, const entry e) {
	char* key = '\0';
	char* value = '\0';
	strcpy(key, e.key);
	strcpy(value, e.value);
	return insert(t, key, value);
}

char* get(const table* t, const char* key) {
	for (int i = 0; i < t->size; i++) {
		if (strcmp(t->entries[i].key, key) == 0)
			return t->entries[i].value;
	}
	return '\0';
}
