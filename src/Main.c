#include "stdafx.h"
#include "Objects.h"

int main() {
    printf("Awale Game Server\n");

    Awale game;
    initAwale(&game);
    printBoard(&game);

    playMove(&game, 0, 2, false);
    printBoard(&game);

    return 0;
}