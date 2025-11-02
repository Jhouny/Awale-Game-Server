#include "Network.h"

// Function to serialize Awale to Awale_Network
Awale_Network serializeAwale(const Awale* game) {
    Awale_Network netGame;
    // Memcpy board
    memcpy(netGame.board, game->board, sizeof(game->board));
    // Copy scores and other fields
    netGame.scores[0] = (uint8_t)game->scores[0];
    netGame.scores[1] = (uint8_t)game->scores[1];
    netGame.gameOver = game->gameOver ? 1 : 0;
    netGame.winner = (int8_t)game->winner;
    return netGame;
}

// Function to deserialize Awale_Network to Awale
void deserializeAwale(const Awale_Network* netGame, Awale* game) {
    // Memcpy board
    memcpy(game->board, netGame->board, sizeof(game->board));
    // Copy scores and other fields
    game->scores[0] = (int)netGame->scores[0];
    game->scores[1] = (int)netGame->scores[1];
    game->gameOver = netGame->gameOver != 0;
    game->winner = (int)netGame->winner;
}

// Function to serialize Move to Move_Network
Move_Network serializeMove(const Move* move) {
    Move_Network netMove;
    netMove.player = (uint8_t)move->player;
    netMove.pit = (uint8_t)move->pit;
    return netMove;
}

// Function to deserialize Move_Network to Move
void deserializeMove(const Move_Network* netMove, Move* move) {
    move->player = (int)netMove->player;
    move->pit = (int)netMove->pit;
}