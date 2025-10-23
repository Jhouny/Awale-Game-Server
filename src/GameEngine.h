#ifndef _GAMEENGINE_H_
#define _GAMEENGINE_H_

#include "stdafx.h"
#include "Objects.h"
#include "Set.h"

bool playMove(Awale* game, int player, int pit, bool recursive);
// Recursive function to check if the current state can lead to captures
bool canCapture(Awale* currentState, Set* seenStates, int player) {
    // Base case: if currentState is in seenStates, return false to avoid loops
    int hash = getHashCode(currentState);
    if (isInSet(seenStates, hash)) {
        return false;
    }
    addToSet(seenStates, hash);
    // Check all possible moves from the current state
    for (int pit = 0; pit < 6; ++pit) {
        if (currentState->board[player][pit] == 0) {
            continue; // Skip empty pits
        }
        Awale nextState;
        copyAwale(currentState, &nextState);
        if (playMove(&nextState, player, pit, false)) {
            // Check if this move results in captures
            int opponent = 1 - player;
            for (int i = 0; i < 6; ++i) {
                if (nextState.board[opponent][i] == 2 || nextState.board[opponent][i] == 3) {
                    return true; // Capture possible
                }
            }
            // Recur for the next state
            if (canCapture(&nextState, seenStates, opponent)) {
                return true;
            }
        }
    }

    return false;
}
#endif