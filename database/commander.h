#ifndef _COMMANDER_H_
#define _COMMANDER_H_

#include "common.h"
#include "Network.h"
#include "database.h"

/**
 * \brief Responsible for evaluating and executing commands received from clients.
 * 
 * \param cmd 
 * \param client_socket_fd 
 * \return Response* 
 */
Response* execute_command(const Command* cmd, int client_socket_fd);

typedef struct commanderGlobals {
	database*               db;
	table*            user_fds;  // Stores current session's file descriptors to username associations
} commanderGlobals;

extern commanderGlobals cmdGlobals;

#endif
