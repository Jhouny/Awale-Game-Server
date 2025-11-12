/* Serveur sockets TCP
 * affichage de ce qui arrive sur la socket
 *    socket_server port (port > 1024 sauf root)
 */

#include "common.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>

// Define global variables here
#define DATABASE_FILE 			"database.db"
#define DATABASE_BACKUP_FILE 	"database.db.bckp"
#define DEFAULT_PORT			"3000"


typedef struct Arguments {
	char port[6];  // Max port number = 65,535
	char db_file[4097];  // Max file path in Linux
} Arguments;

Arguments globals;

/**
 * \brief Function to parse the terminal arguments passed to the server
 */
int parse_args(int argc, char* argv[]);

/**
 * \brief Encapsulates the server startup logic. Deals with socket creation and binding to port.
 */
int setup_server();
