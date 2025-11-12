#include "app_statemachine.h"
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
            printf("Enter your username...\n");
            printf("Enter your password...\n");
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
            printf("  5. View challenges");
            if (challenges > 0) {
                printf(" \033[33m\033[1m(%d incoming)\033[0m", challenges);
            }
            printf("\n");

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
        
        case STATE_CHALLENGE:
            printf("CHALLENGE MENU\n\n");
            printf("Enter the username of the player you wish to challenge:\n");
            printf("Or type 'back' to return to home.\n");
            break;
        
        case STATE_WRITE_BIO:
            printf("WRITE BIO\n\n");
            printf("Enter your new bio below (single line):\n");
            printf("Type 'cancel' to discard changes.\n");
            printf("Or type 'back' to return to home.\n");
            break;

        case STATE_CHOOSE_GAME_SPECTATE:
            printf("CHOOSE GAME TO SPECTATE\n\n");
            printf("List of active games:\n");
            printf("Type 'select <id>' to spectate a game.\n");
            printf("Type 'back' to return to home.\n");
            break;  

        case STATE_SPECTATING:
        printf("SPECTATING GAME\n\n");
        printf("You are now watching an ongoing Awale match.\n");
        printf("  - Type 'back' to stop spectating.\n");
        break;

        case STATE_IN_GAME: // Corresponds to IN_GAME
            printf("IN GAME\n\n");
            // Awale board rendering
            printf("Awale board display...\n");
            break;
        
            case STATE_CHOOSE_GAME_TO_REVIEW:
        printf("CHOOSE GAME TO REVIEW\n\n");
        printf("List of past games:\n");
        printf("(simulated list)\n");
        printf("Type 'review <id>' to see moves.\n");
        printf("Type 'back' to return to home.\n");
        break;

        case STATE_REVIEWING:
            printf("REVIEWING GAME\n\n");
            printf("Use commands:\n");
            printf("  - 'next' → go to next move\n");
            printf("  - 'prev' → go back one move\n");
            printf("  - 'exit' → return to home\n");
            break;

        case STATE_RETRIEVE_BIO:
            printf("RETRIEVE BIO\n\n");
            printf("Enter the username of the player whose bio you want to read:\n");
            printf("Type 'back' to return to home.\n");
            break;

        case STATE_CHOOSE_CHAT:
            printf("CHOOSE CHAT\n\n");
            printf("Enter the username of the person you want to chat with:\n");
            printf("Or type 'back' to return to home.\n");
            break;

        case STATE_CHATTING:
            printf("CHATTING\n\n");
            printf("You are now chatting with another user.\n");
            printf("Type your messages directly.\n");
            printf("Type 'back' to close the chat.\n");
            break;

        case STATE_FRIENDS:
            printf("FRIENDS LIST\n\n");
            printf("Here are your friends:\n");
            printf("Commands:\n");
            printf("  - 'add <username>' to add a friend\n");
            printf("  - 'remove <username>' to remove a friend\n");
            printf("  - 'back' to return to home\n");
            break;

        case STATE_EXIT:
            printf("EXITING CLIENT...\n");
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

void renderer_state_loop(int sockfd) {
    // Client data initialization
    ClientData client_data = {
        .current_state = STATE_HOME,
        .incoming_challenges_count = 0
    };
    
    // Counter to simulate server updates
    int server_update_counter = 0;
    int show_notification = 0;
    
    printf("Client running. Type 'exit' to quit.\n");

    ClientState previous_state = client_data.current_state; // memorise previous state
   int need_refresh = 1;

    while (client_data.current_state != STATE_EXIT) {
        if (need_refresh || previous_state != client_data.current_state) {
            render_client_state(&client_data);
            previous_state = client_data.current_state;
            need_refresh = 0;
        }

        if (show_notification) {
                fflush(stdout);
                show_notification = 0;
            }
    
        // 2. Configuration for select()
        fd_set read_fds;
        FD_ZERO(&read_fds);
        /*A REMETTRE*/
        /*FD_SET(sockfd, &read_fds);*/       // Monitors server socket
        FD_SET(STDIN_FILENO, &read_fds); // Monitors user input (terminal)

        // 3. Defining the Timeout (your 5-second refresh)
        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        /* A REMETTRE*/
        /*int max_fd = (sockfd > STDIN_FILENO) ? sockfd : STDIN_FILENO;*/
        // select blocks until data arrives OR timeout is reached (5s)
        /*int activity = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);*/
        int activity = select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout);


        if (activity < 0) {
            perror("Select error");
            break;
        }

        // 4. Handling User Input (STDIN)
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            char input[256];
            if (fgets(input, sizeof(input), stdin) != NULL) {
                input[strcspn(input, "\n")] = 0; 
                
                // Commande de sortie
                if (strcmp(input, "exit") == 0 || strcmp(input, "9") == 0) {
                    client_data.current_state = STATE_EXIT;
                }
                // --- HOME menu commands ---
                else if (client_data.current_state == STATE_HOME) {
                    if (strcmp(input, "1") == 0) {
                        client_data.current_state = STATE_CHALLENGE;
                    } 
                    else if (strcmp(input, "2") == 0) {
                        client_data.current_state = STATE_WRITE_BIO;
                    } 
                    else if (strcmp(input, "3") == 0) {
                        client_data.current_state = STATE_CHOOSE_GAME_SPECTATE;
                    } 
                    else if (strcmp(input, "4") == 0) {
                        client_data.current_state = STATE_CHOOSE_GAME_TO_REVIEW;
                    } 
                    else if (strcmp(input, "5") == 0) {
                        client_data.current_state = STATE_VIEW_CHALLENGES;
                    } 
                    else if (strcmp(input, "6") == 0) {
                        client_data.current_state = STATE_RETRIEVE_BIO;
                    } 
                    else if (strcmp(input, "7") == 0) {
                        client_data.current_state = STATE_CHOOSE_CHAT;
                    } 
                    else if (strcmp(input, "8") == 0) {
                        client_data.current_state = STATE_FRIENDS;
                    }
                }
                 // --- Back command: return to HOME from any state ---
                else if (strcmp(input, "back") == 0) {
                    // Special handling for VIEW_CHALLENGES: reset counter
                    if (client_data.current_state == STATE_VIEW_CHALLENGES) {
                        client_data.incoming_challenges_count = 0;
                    }
                    client_data.current_state = STATE_HOME;
                }

                // Force rafraîchissement après saisie
                need_refresh = 1;
            }
        }


        /*A COMMENTER APRES */
        // 4. Simulated socket behavior when no server is running
        if (sockfd != -1 && FD_ISSET(sockfd, &read_fds)) {
            // Skip socket reads if not connected (demo mode)
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

            server_update_counter++;
                if (server_update_counter % 3 == 0) {
                    client_data.incoming_challenges_count++;
                    show_notification = 1; // Active le flag
                    need_refresh = 1;
                }
            
            
        }
        usleep(50000);
    }
}
