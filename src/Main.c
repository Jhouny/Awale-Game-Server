#include "stdafx.h"
#include "Objects.h"
#include "Set.h"
#include "GameEngine.h"

int main() {
    printf("Awale Game Server\n");

    Awale game;
    initAwale(&game);
    /*printBoard(&game);
    while (1) {
        int player, pit;
        printf("Enter player (0 or 1) and pit (0-5): ");
        scanf("%d %d", &player, &pit);

        if (!playMove(&game, player, pit, false)) {
            printf("Invalid move. Try again.\n");
        }

        printBoard(&game);
    }*/
   game.board[0][0] = 0;
   game.board[0][1] = 0;
   game.board[0][2] = 0;
   game.board[0][3] = 0;
   game.board[0][4] = 1;
   game.board[0][5] = 0;
   game.board[1][0] = 0;
   game.board[1][1] = 1;
   game.board[1][2] = 0;
   game.board[1][3] = 0;
   game.board[1][4] = 0;
   game.board[1][5] = 0;

   Set seenStates;
   initSet(&seenStates, 10);

   if (canCapture(&game, &seenStates, 0))
   {
       printf("Player 0 can capture in future moves.\n");
   }
   else
   {
       printf("Player 0 cannot capture in future moves.\n");
   }
   return 0;
}