#ifndef _GAMEENGINE_H_
#define _GAMEENGINE_H_

#include "common.h"
#include "Objects.h"
#include "Set.h"

bool playMove(Awale* game, int player, int pit, bool recursive);
// Recursive function to check if the current state can lead to captures
bool canCapture(Awale* currentState, Set* seenStates, int player);
#endif
