/* Serveur sockets TCP
 * affichage de ce qui arrive sur la socket
 *    socket_server port (port > 1024 sauf root)
 */

#include "common.h"
#include <signal.h>

#define DEFAULT_PORT	"3001"
#define DEFAULT_DB_PORT	"3000"
#define DEFAULT_DB_ADDR	"127.0.0.1"

typedef struct Arguments {
	char port[6];  // Max port number = 65,535
	char db_addr[256];  // Database address
	char db_port[6];  // Database port
} Arguments;

Arguments globals;
struct pollfd* client_sockets_fd = NULL;
size_t total_clients = 0;
pthread_mutex_t mut_client_sockets_fd = PTHREAD_MUTEX_INITIALIZER;

/**
 * \brief Function to parse the terminal arguments passed to the server
 */
int parse_args(int argc, char* argv[]);

/**
 * \brief Encapsulates the server startup logic. Deals with socket creation and binding to port.
 */
int setup_server();

/**
 * \brief Function to handle reallocation for new clients.
 */
int add_client(int new_client_sfd);

/**
 * \brief Function to handle reallocation for when client disconnects.
 */
int remove_client(int client_sfd);

/**
 * \brief Thread function to accept new incoming connections.
 */
void *connection_loop(void *param);

/**
 * \brief Thread function to periodically save the database to disk.
 */
void *database_save_loop(void *param);