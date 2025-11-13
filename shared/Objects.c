#include "Objects.h"

void initAwale(Awale* game) {
    // Initialize the board with 4 seeds in each pit
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 6; ++j) {
            game->board[i][j] = 4;
        }
        game->scores[i] = 0;
    }

    game->gameOver = false;
    game->winner = -1;
    game->game_id = -1;
    memset(game->playernames, 0, sizeof(game->playernames));
    game->lastPlayer[0] = '\0';
    game->spectators_fds = NULL;
    game->spectators_count = 0;
}

// Function to copy one awale game state to another
void copyAwale(const Awale* src, Awale* dest) {
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 6; ++j) {
            dest->board[i][j] = src->board[i][j];
        }
        dest->scores[i] = src->scores[i];
    }
    dest->gameOver = src->gameOver;
    dest->winner = src->winner;
}

// Function to print the current board state
void printBoard(Awale* game) {
    // Implementation of board printing goes here
    for (int i = 0; i < 2; ++i) {
        printf("Player %d: ", i);
        for (int j = 0; j < 6; ++j) {
            printf("%d ", game->board[i][j]);
        }
        printf(" | Score: %d\n", game->scores[i]);
    }
}

// Function to generate the hash code for the current game state
int getHashCode(Awale* game) {
    int hash = 0;
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 6; ++j) {
            hash = hash * 31 + game->board[i][j];
        }
    }
    return hash;
}
