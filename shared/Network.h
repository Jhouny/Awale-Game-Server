#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "common.h"
#include "Objects.h"

typedef struct Awale_Network {
    // Represents the network state of an Awale game
    uint8_t board[2][6]; // 2 players, 6 pits each
    uint8_t scores[2];   // Scores for each player
    uint8_t gameOver; // int for portability over the network (padding)
    int8_t winner; // -1 if no winner, 0 or 1 for players
} Awale_Network;

Awale_Network serializeAwale(const Awale* game);
void deserializeAwale(const Awale_Network* netGame, Awale* game);

typedef struct Move_Network {
    // Represents a move in the Awale game over the network
    uint8_t player; // 0 or 1
    uint8_t pit;    // 0 to 5
} Move_Network;

Move_Network serializeMove(const Move* move);
void deserializeMove(const Move_Network* netMove, Move* move);

#endif
