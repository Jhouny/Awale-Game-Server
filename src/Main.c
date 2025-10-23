#include "stdafx.h"
#include "Objects.h"
#include "GameEngine.h"

int main() {
    printf("Awale Game Server\n");

    Awale game;
    initAwale(&game);
    printBoard(&game);
    while (1) {
        int player, pit;
        printf("Enter player (0 or 1) and pit (0-5): ");
        scanf("%d %d", &player, &pit);

        if (!playMove(&game, player, pit, false)) {
            printf("Invalid move. Try again.\n");
        }

        printBoard(&game);
    }

    return 0;
}