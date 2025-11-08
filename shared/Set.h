#ifndef _SET_H_
#define _SET_H_

#include "common.h"

// Represents a general set data structure for various uses 
typedef struct Set {
    int* elements;
    int size;
    int capacity;
} Set;

void initSet(Set* set, int capacity);
bool addToSet(Set* set, int element);
bool isInSet(Set* set, int element);

#endif
