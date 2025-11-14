#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "common.h"
#include "Objects.h"


#define MAX_ARG_LEN	250
#define MAX_CMD_LEN	32


typedef struct Awale_Network {
    // Represents the network state of an Awale game
    uint8_t board[2][6]; // 2 players, 6 pits each
    uint8_t scores[2];   // Scores for each player
    uint8_t gameOver; // int for portability over the network (padding)
    char lastPlayer[250]; // The player who made the last move
    char playernames[2][250];
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

typedef struct Command {
	char command[MAX_CMD_LEN];
	char* args[MAX_ARG_LEN];
	int args_size;
} Command;

Command* createCommand(char* command, char** args, int size);
int serialize_and_send_Command(int socket_fd, Command* cmd);
Command* receive_and_deserialize_Command(int socket_fd);

typedef struct Response {
    int status_code; // 1 for success, 0 for error
    int message_size;
    char message[MAX_ARG_LEN];  // Addtional info or error message
    int body_size;
    char *body[MAX_ARG_LEN];  // Serialized data objects
} Response;

int serialize_and_send_Response(int socket_fd, Response* res);
Response* receive_and_deserialize_Response(int socket_fd);

#endif
