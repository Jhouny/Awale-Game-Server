#include "GameEngine.h"

bool playMove(Awale* game, int player, int pit, bool recursive) {
    Awale tempGame;
    copyAwale(game, &tempGame);

    // Implementation of move logic goes here
    if (player < 0 || player > 1 || pit < 0 || pit > 5)
    {
        return false; // Invalid move
    }

    if (tempGame.board[player][pit] == 0)
    {
        return false; // Cannot play from an empty pit
    }

    // Distribute seeds
    int seeds = tempGame.board[player][pit];
    tempGame.board[player][pit] = 0;
    int index = pit;
    int currentPlayer = player;
    int direction = (currentPlayer == 0) ? 1 : -1;
    while (seeds > 0)
    {
        index += direction;
        if (index >= 6)
        {
            index = 5;
            currentPlayer = 1 - currentPlayer; // Switch sides
            direction = (currentPlayer == 0) ? 1 : -1;
        } else if (index < 0)
        {
            index = 0;
            currentPlayer = 1 - currentPlayer; // Switch sides
            direction = (currentPlayer == 0) ? 1 : -1;
        }

        if (index == pit && currentPlayer == player)
            continue; // Skip the starting pit

        tempGame.board[currentPlayer][index]++;
        seeds--;
    }

    // Verify board state and capture seeds if applicable
    int opponent = 1 - player;
    for (int i = index; i >= 0 && i < 6;)
    {
        if (tempGame.board[opponent][i] == 2 || tempGame.board[opponent][i] == 3)
        {
            tempGame.scores[player] += tempGame.board[opponent][i];
            tempGame.board[opponent][i] = 0;
        }
        else
        {
            break; // Stop capturing if a pit doesn't have 2 or 3 seeds
        }
        i -= direction;
    }

    // Validate game rules
    //  - Ensure the opponent has seeds to play
    //  - Check for endgame conditions
    if (tempGame.scores[opponent] >= 25)
    {
        // Opponent wins
        tempGame.gameOver = true;
        tempGame.winner = opponent;
    }
    else if (tempGame.scores[player] >= 25)
    {
        // Current player wins
        tempGame.gameOver = true;
        tempGame.winner = player;
    }

    // Check if opponent has no seeds left
    bool opponentHasSeeds = false;
    for (int i = 0; i < 6; ++i)
    {
        if (tempGame.board[opponent][i] > 0)
        {
            opponentHasSeeds = true;
            break;
        }
    }

    if (!opponentHasSeeds)
    {
        // Invalid move as it would starve the opponent
        // Verify if there is any valid move that can feed the opponent
        bool validMoveExists = false;
        for (int i = 0; i < 6; ++i)
        {
            if (tempGame.board[player][i] > 0)
            {
                // Recursive check if this move can feed the opponent
                Awale testGame = *game;
                if (playMove(&testGame, player, i, true))
                {
                    validMoveExists = true;
                    break;
                }
            }
        }

        if (!validMoveExists)
        {
            // No valid moves available to feed the opponent
            tempGame.gameOver = true;
            if (tempGame.scores[player] > tempGame.scores[opponent])
            {
                tempGame.winner = player;
            }
            else if (tempGame.scores[player] < tempGame.scores[opponent])
            {
                tempGame.winner = opponent;
            }
            else
            {
                tempGame.winner = -1; // Draw
            }
        }
        else
        {
            return false; // Move is invalid as it starves the opponent
        }
    }

    // Check if game over due to not being able to capture more seeds

    // Copy the temporary board back to the main board
    if (!recursive) {
        copyAwale(&tempGame, game);
    }

    return true; // Return true if the move was successful
}