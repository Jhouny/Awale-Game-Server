#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include "common.h"

// Represents the state of an Awale game
typedef struct Awale {
    uint8_t      board[2][6];  // 2 players, 6 pits each
    uint8_t        scores[2];  // Scores for each player
    bool            gameOver;
    int8_t            winner;  // -1 if no winner, 0 or 1 for players
    int              game_id;  // Unique identifier for the game
    char playernames[2][250];  // Names of the two players
    int        currentPlayer;
    int      *spectators_fds;  // Dynamic array of spectator file descriptors
    size_t  spectators_count;
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
