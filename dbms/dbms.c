#include "dbms.h"
#include "Network.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <IP> <PORT>\n", argv[0]);
        return 1;
    }

    int socket_fd;
    struct addrinfo hints;
    struct addrinfo* result;
    struct addrinfo* rp;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;      // IPv4 ou IPv6
    hints.ai_socktype = SOCK_STREAM;  // TCP

    //  Résolution adresse
    if (getaddrinfo(argv[1], argv[2], &hints, &result) != 0) {
        printf("Error getting address info.\n");
        return 1;
    }

    // Tentative de connexion
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        socket_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (socket_fd == -1)
            continue;
        if (connect(socket_fd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;
        close(socket_fd);
    }

    freeaddrinfo(result);

    if (rp == NULL) {
        printf("Couldn't connect to any address.\n");
        return 1;
    }

    printf("Connected to %s:%s\n", argv[1], argv[2]);

    struct termios orig_termios;

    enable_alternate_buffer();

    // Main loop or functionality would go here
    int running = 1;
    printf("DBMS is running... (Type 'exit' to quit)\n");
    while (running) {
        printf(">>> ");
        char input[256];
        if (fgets(input, sizeof(input), stdin)) {
            input[strcspn(input, "\n")] = 0;  // Remove newline
            // Remove trailing spaces
            char* end = input + strlen(input) - 1;
            while (end > input && *end == ' ') {
                *end = '\0';
                end--;
            }

            if (strcmp(input, "exit") == 0) {
                running = 0;
            } else {
                // Handle command
                char cmd[MAX_CMD_LEN];
                char* args[MAX_ARG_LEN];
                int args_size = 0;
                char* token = strtok(input, " ");
                if (token != NULL) {
                    strncpy(cmd, token, MAX_CMD_LEN);
                    cmd[MAX_CMD_LEN - 1] = '\0';
                    // Parse arguments
                    while ((token = strtok(NULL, " ")) != NULL) {
                        args[args_size] = token;
                        args_size++;
                    }
                } else {
                    continue;  // Empty input
                }
                Command* command = createCommand(cmd, args, args_size);
                int res = serialize_and_send_Command(socket_fd, command);
                if (res == 0) {
                    Response* response = receive_and_deserialize_Response(socket_fd);
                    if (response) {
                        if (response->status_code == 0) {
                            printf("Command failed. Server's response: %s\n", response->message);
                        } else {
                            for (int i = 0; i < response->body_size; i++) {
                                printf(" - %d : %s\n", i, response->body[i]);
                            }
                        }
                        for (int i = 0; i < response->body_size; i++)
                            free(response->body[i]);
                        free(response);
                    } else {
                        printf("ERROR: No response from server.\n");
                    }
                } else {
                    printf("ERROR: Failed to send command '%s'.\n", cmd);
                }
            }
        }
    }
    
    disable_alternate_buffer();
    close(socket_fd);
    return 0;
}