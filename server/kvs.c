#include "kvs.h"

entry* create_entry() {
	entry* e = (entry*) malloc(sizeof(entry));
	if (e == NULL)
		return NULL;
	e->free = 1;
	return e;
}

int delete_entry(entry* e, int locked) {
	if (!locked)
		pthread_mutex_lock(&mut_database);
	if (e == NULL) {
		if (!locked) {
			pthread_mutex_unlock(&mut_database);
		return 0;
	}
	free(e);
	if (!locked)
		pthread_mutex_unlock(&mut_database);
	return 1;
}

table* create_table() {
	table* t = (table*) malloc(sizeof(table));
	if (t == NULL)
		return NULL;
	t->size = 20;
	t->entries = (entry**) malloc(t->size * sizeof(entry*));
	if (t->entries == NULL) {
		free(t);
		return NULL;
	}
	for (int i = 0; i < t->size; i++) {
		t->entries[i] = create_entry();
	}
	t->name[0] = '\0';
	return t;
}

int delete_table(table* t, int locked) {	
	if (!locked)
		pthread_mutex_lock(&mut_database);
	if (t == NULL) {
		if (!locked)
			pthread_mutex_unlock(&mut_database);
		return 0;
	}
	for (int i = 0; i < t->size; i++) {
		delete_entry(t->entries[i]);
	}
	free(t->entries);
	free(t);
	if (!locked)
		pthread_mutex_unlock(&mut_database);
	return 1;
}

void set_table_name(table* t, const char* name) {
	pthread_mutex_lock(&mut_database);
	if (name == NULL || t == NULL) {
		pthread_mutex_unlock(&mut_database);
		return;
	}
	strncpy(t->name, name, 31);
	t->name[31] = '\0';
	pthread_mutex_unlock(&mut_database);
}

int insert(table* t, const char* key, const char* value) {
	pthread_mutex_lock(&mut_database);
	if (t == NULL || key == NULL || value == NULL) {
		pthread_mutex_unlock(&mut_database);
		return 0;
	}
	if (strcmp(key, "\0") == 0) {
		pthread_mutex_unlock(&mut_database);
		return 0;
	}

	// Check if key already exists in table. If not, add it.
	for (int i = 0; i < t->size; i++) {
		entry* curr = t->entries[i];
		if (!curr->free && strcmp(curr->key, key) == 0) {
			// Key already exists, update value
			strncpy(curr->value, value, MAX_VALUE_LEN - 1);
			curr->value[MAX_VALUE_LEN - 1] = '\0';
			pthread_mutex_unlock(&mut_database);
			return 1;
		}
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
		if (t->entries == NULL) {
			t->size = prevSize; // Restore previous size on failure
			pthread_mutex_unlock(&mut_database);
			return 0;
		}
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

	pthread_mutex_unlock(&mut_database);
	return 1;
}

int insert_entry(table* t, const entry* e) {
	if (e == NULL)
		return 0;
	return insert(t, e->key, e->value);
}

int remove_entry(table* t, const char* key) {
	pthread_mutex_lock(&mut_database);
	if (t == NULL || key == NULL) {
		pthread_mutex_unlock(&mut_database);
		return 0;
	}
	
	for (int i = 0; i < t->size; i++) {
		if (!t->entries[i]->free && strcmp(t->entries[i]->key, key) == 0) {
			t->entries[i]->free = 1; // Mark as free
			pthread_mutex_unlock(&mut_database);
			return 1;
		}
	}
	pthread_mutex_unlock(&mut_database);
	return 0; // Not found
}

char* get(const table* t, const char* key) {
	pthread_mutex_lock(&mut_database);
	if (t == NULL || key == NULL) {
		pthread_mutex_unlock(&mut_database);
		return NULL;
	}
		
	for (int i = 0; i < t->size; i++) {
		if (!t->entries[i]->free && strcmp(t->entries[i]->key, key) == 0) {
			pthread_mutex_unlock(&mut_database);
			return t->entries[i]->value;
		}
	}
	pthread_mutex_unlock(&mut_database);
	return NULL;
}
