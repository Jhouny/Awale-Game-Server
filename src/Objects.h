#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include "stdafx.h"

typedef struct Awale {
    // Represents the state of an Awale game
    int board[2][6]; // 2 players, 6 pits each
    int scores[2];   // Scores for each player
    bool gameOver;
    int winner; // -1 if no winner, 0 or 1 for players
} Awale;

void initAwale(Awale* game);
void copyAwale(const Awale* src, Awale* dest);
void printBoard(Awale* game);
int getHashCode(Awale* game);

typedef struct Move {
    // Represents a move in the Awale game
    int player; // 0 or 1
    int pit;    // 0 to 5
} Move;

#endif
