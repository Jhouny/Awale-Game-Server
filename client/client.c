#include "client.h"

void clear_terminal() {
    // \033[2J: Clear screen
    // \033[H: Move cursor to home (top-left)
    printf("\033[2J\033[H");
}

void render_client_state(const ClientData* data) {
    clear_terminal();

    printf("--- Awale Game Client ---\n");
    
    // --- State-Specific Rendering ---
    switch (data->current_state) {
        case STATE_LOGIN:
            printf("LOGIN\n\n");
            printf("Enter your username and password...\n");
            break;

        case STATE_HOME:
            printf("HOME\n\n");
            printf("Welcome to the Awale server!\n\n");
            
            // --- Notification Badge Logic ---
            int challenges = data->incoming_challenges_count;
            
            printf("  1. Launch a challenge\n");
            printf("  2. Write Bio\n");
            printf("  3. Spectate a game\n");
            printf("  4. Review saved games\n");
            
            // Conditional display of the badge (1) in yellow/bold
            printf("  5. VIEW CHALLENGES %s\n", (challenges > 0) ? 
                   "(\033[33m\033[1m%d incoming\033[0m)" : 
                   "");

            printf("  6. Retrieve Bio\n");
            printf("  7. Chat\n");
            printf("  8. Friends\n");
            printf("  9. Logout (type 'exit')\n");

            break;
            
        case STATE_VIEW_CHALLENGES:
            printf("INCOMING CHALLENGES\n\n");
            if (data->incoming_challenges_count > 0) {
                printf("You have \033[33m\033[1m%d\033[0m pending challenge(s).\n", data->incoming_challenges_count);
                // Logic to list challenges
            } else {
                printf("You have no pending challenges.\n");
            }
            printf("Type 'back' to return to home.\n");
            break;

        case STATE_IN_GAME: // Corresponds to IN_GAME
            printf("IN GAME\n\n");
            // Awale board rendering
            printf("Awale board display...\n");
            break;

        default:
            printf("UNDEFINED STATE\n\n");
            break;
    }

    // User prompt line
    printf("\n-----------------------------------\n");
    printf("Enter your command: ");
    fflush(stdout); // Ensure text is immediately displayed
}

void client_state_loop(int sockfd) {
    // Client data initialization
    ClientData client_data = {
        .current_state = STATE_HOME,
        .incoming_challenges_count = 0
    };
    
    // Counter to simulate server updates
    int server_update_counter = 0;
    
    printf("Client running. Type 'exit' to quit.\n");

    ClientState previous_state = client_data.current_state; // mémorise état précédent
   int need_refresh = 1;

    while (client_data.current_state != STATE_EXIT) {
        if (need_refresh || previous_state != client_data.current_state) {
            render_client_state(&client_data);
            previous_state = client_data.current_state;
            need_refresh = 0;
        }
        
    
        // 2. Configuration for select()
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);       // Monitors server socket
        FD_SET(STDIN_FILENO, &read_fds); // Monitors user input (terminal)

        // 3. Defining the Timeout (your 5-second refresh)
        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        int max_fd = (sockfd > STDIN_FILENO) ? sockfd : STDIN_FILENO;
        // select blocks until data arrives OR timeout is reached (5s)
        int activity = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);

        if (activity < 0) {
            perror("Select error");
            break;
        }
        /*A DECOMMENTER APRES*/
        // 4. Handling User Input (STDIN)
        /*if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            char input[256];
            if (fgets(input, sizeof(input), stdin) != NULL) {
                input[strcspn(input, "\n")] = 0; 
                
                // Input processing logic:
                if (strcmp(input, "exit") == 0) {
                    client_data.current_state = STATE_EXIT;
                } else if (client_data.current_state == STATE_HOME) {
                    if (strcmp(input, "5") == 0) {
                        client_data.current_state = STATE_VIEW_CHALLENGES;
                    }
                } else if (client_data.current_state == STATE_VIEW_CHALLENGES) {
                    if (strcmp(input, "back") == 0) {
                        // Reset challenge counter after viewing them
                        client_data.incoming_challenges_count = 0; 
                        client_data.current_state = STATE_HOME;
                    }
                }
                // ... Add logic for other states and commands
            }
        }*/

        /*A COMMENTER APRES */
        // 4. Simulated socket behavior when no server is running
        if (sockfd != -1 && FD_ISSET(sockfd, &read_fds)) {
            // 🔒 Skip socket reads if not connected (demo mode)
            struct sockaddr_in addr;
            socklen_t len = sizeof(addr);
            int connected = getpeername(sockfd, (struct sockaddr *)&addr, &len);
            if (connected == -1) {
                // Not connected — skip reading
                continue;
            }

            char buffer[512];
            int n = read(sockfd, buffer, 511);
            if (n > 0) {
                buffer[n] = '\0';
                if (strstr(buffer, "CHALLENGE_INCOMING")) {
                    client_data.incoming_challenges_count++;
                }
            } else if (n == 0) {
                printf("\nServer closed connection.\n");
                client_data.current_state = STATE_EXIT;
            } else {
                perror("Socket read error");
                client_data.current_state = STATE_EXIT;
            }
        }

        /*A DECOMMENTER APRES */
        // 5. Handling Server Messages (SOCKET)
        /*if (FD_ISSET(sockfd, &read_fds)) {
            char buffer[512];
            int n = read(sockfd, buffer, 511);
            if (n > 0) {
                buffer[n] = '\0';
                // You must PARSE the server message and UPDATE client_data.
                // Ex: If server sends "CHALLENGE_INCOMING:userB"
                if (strstr(buffer, "CHALLENGE_INCOMING")) {
                    client_data.incoming_challenges_count++;
                }
            } else if (n == 0) {
                printf("\nServer closed connection.\n");
                client_data.current_state = STATE_EXIT;
            } else {
                perror("Socket read error");
                client_data.current_state = STATE_EXIT;
            }
        }*/
        
        // 6. Handling Timeout (Simulated Refresh/Update)
        if (activity == 0) {
            // The 5-second timeout has been reached.
            // Rendering is called at the start of the loop, so it will refresh the screen.
            
            // SIMULATION OF NEW CHALLENGE (for testing the badge)
            if (client_data.current_state == STATE_HOME && server_update_counter % 3 == 0) {
                // Simulate receiving a challenge every 3 iterations (15 seconds)
                 client_data.incoming_challenges_count += (server_update_counter == 0) ? 0 : 1;
            }
            server_update_counter++;
             need_refresh = 1;
        }
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            // après avoir traité une commande utilisateur
            need_refresh = 1;
        }

        if (FD_ISSET(sockfd, &read_fds)) {
            // après avoir lu un message serveur
            need_refresh = 1;
        }
    }
}

int main(int argc, char** argv ) {
    int sockfd, newsockfd, clilen, chilpid, ok, nleft, nbwriten;
    char c;
    struct sockaddr_in cli_addr, serv_addr;

    if (argc != 3) {
        printf("usage  socket_client server port\n");
        exit(0);
    }

    printf ("client starting\n");  

    /* initialise la structure de donnee */
    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    struct hostent *he = gethostbyname(argv[1]);
    if (he == NULL) {
        perror("gethostbyname");
        exit(1);
    }
    memcpy(&serv_addr.sin_addr, he->h_addr_list[0], he->h_length);
    serv_addr.sin_port = htons(atoi(argv[2]));

    printf("Connecting to %s:%s\n", inet_ntoa(serv_addr.sin_addr), argv[2]);


    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if (sockfd == -1) {
        printf("socket error\n");
        exit(0);
    }

    /*DECOMMENTER APRES*/
    /* effectue la connection */
    //if (connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
    //{
        //perror("connect");
        //printf("socket EEerror\n");
        //exit(EXIT_FAILURE);
    //}

    printf("Connection successful.\n");

    // --- NEW LOGIC: Start the state/render loop ---
    client_state_loop(sockfd);
    
    // Close socket on exit
    close(sockfd);
    printf("Client disconnected.\n");

    return 0;
}
