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
 * \return int 
 */
int execute_command(database* db, const Command* cmd, int client_socket_fd);

#endif