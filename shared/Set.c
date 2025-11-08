#include "Set.h"

void initSet(Set* set, int capacity) {
    set->elements = (int*)malloc(sizeof(int) * capacity);
    set->size = 0;
    set->capacity = capacity;
}

bool addToSet(Set* set, int element) {
    if (isInSet(set, element)) {
        return false; // Element already in set
    }
    if (set->size >= set->capacity) {
        // Resize the array if capacity is reached
        set->capacity *= 2;
        set->elements = (int*)realloc(set->elements, sizeof(int) * set->capacity);
    }
    set->elements[set->size++] = element;
    return true;
}

bool isInSet(Set* set, int element) {
    for (int i = 0; i < set->size; i++) {
        if (set->elements[i] == element) {
            return true;
        }
    }
    return false;
}
