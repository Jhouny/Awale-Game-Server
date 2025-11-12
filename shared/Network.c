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

// Function to initialize a command struct
Command* createCommand(char* command, char** args, int size) {
	Command* cmd = (Command*) malloc(sizeof(Command));
	if (cmd == NULL) {
		printf("Error allocating command.\n");
		return NULL;
	}

	strncpy(cmd->command, command, 32);
	cmd->command[31] = '\0';

	cmd->args_size = size;
	for (int i = 0; i < size; i++) {
		cmd->args[i] = (char*) malloc(sizeof(char) * MAX_ARG_LEN);
		if (cmd->args[i] == NULL) {
			printf("Error allocating command argument.\n");
			free(cmd);
			return NULL;
		}
		strncpy(cmd->args[i], args[i], MAX_ARG_LEN);
		cmd->args[i][MAX_ARG_LEN - 1] = '\0';
	}
	
	return cmd;
}

// Function to serialize and send a command to the socket
int serialize_and_send_Command(int socket_fd, Command* cmd) {
	size_t total_bytes_sent = 0;
	int argument_len;

	// Send Command name
	if (send(socket_fd, cmd->command, MAX_CMD_LEN, 0) != MAX_CMD_LEN) {
		printf("Error sending command name.\n");
		return -1;
	}
	total_bytes_sent += MAX_CMD_LEN;

	// Send number of arguments
	if (send(socket_fd, &cmd->args_size, sizeof(int), 0) != sizeof(int)) {
		printf("Error sending number of arguments.\n");
		return -1;
	}
	total_bytes_sent += sizeof(int);

	// Send arguments
	for (int i = 0; i < cmd->args_size; i++) {
		if (cmd->args[i] == NULL)
			continue;

		// Send argument
		if (send(socket_fd, cmd->args[i], MAX_ARG_LEN, 0) != (size_t)MAX_ARG_LEN) {
			printf("Error sending argument '%s'.\n", cmd->args[i]);
			return -1;
		}
		total_bytes_sent += MAX_ARG_LEN;
	}

	return 0;
}

Command* receive_and_deserialize_Command(int socket_fd) {
	Command* cmd = (Command*) malloc(sizeof(Command));
	if (cmd == NULL) {
		printf("Error allocating command.\n");
		return NULL;
	}
	memset(cmd, 0, sizeof(Command));

	size_t total_bytes_received = 0;
	int arg_len;

	printf("Receiving command from socket %d.\n", socket_fd);

	// Receive command name
	if (recv(socket_fd, cmd->command, MAX_CMD_LEN, 0) != MAX_CMD_LEN) {
		free(cmd);
		printf("Error receiving command name.\n");
		return NULL;
	}
	total_bytes_received += MAX_CMD_LEN;

	// Receive number of arguments
	if (recv(socket_fd, &cmd->args_size, sizeof(int), 0) != sizeof(int)) {
		free(cmd);
		printf("Error receiving command size.\n");
		return NULL;
	}
	total_bytes_received += sizeof(int);

	printf("Command '%s' with %d arguments.\n", cmd->command, cmd->args_size);
	// Receive arguments 
	for (int i = 0; i < cmd->args_size; i++) {
		cmd->args[i] = (char*) malloc(sizeof(char) * MAX_ARG_LEN);
		if (cmd->args[i] == NULL) {
			free(cmd);
			printf("Error allocating argument buffer.\n");
			return NULL;
		}

		// Receive argument
		if (recv(socket_fd, cmd->args[i], MAX_ARG_LEN, 0) != MAX_ARG_LEN) {
			free(cmd);
			printf("Error receiving argument size.\n");
			return NULL;
		}
		total_bytes_received += MAX_ARG_LEN;
	}

	return cmd;
}





















