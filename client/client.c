#include "client.h"
#include "renderer.h"
#include "Network.h"
#include <netdb.h> 

//Simulate incoming challenges every 10 seconds
void* challenge_simulator(void* arg) {
    ClientData* data = (ClientData*) arg;
    while (data->current_state != STATE_EXIT) {
        sleep(10);
        
        pthread_mutex_lock(&data->data_mutex);
        data->incoming_challenges_count++;
        
        data->challenges_updated = 1; //Notify main loop to redraw
        pthread_mutex_unlock(&data->data_mutex);
    }
    return NULL;
}

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

    // 🔗 Résolution adresse
    if (getaddrinfo(argv[1], argv[2], &hints, &result) != 0) {
        printf("Error getting address info.\n");
        return 1;
    }

    // 🔌 Tentative de connexion
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

    ClientData client_data = {
        .current_state = STATE_LOGIN,
        .incoming_challenges_count = 0,
        .selected_menu_option = 0,
        .challenges_updated = 0,
        .game_count = 0,
        .game_ids = {0},
    };
    pthread_mutex_init(&client_data.data_mutex, NULL);

    pthread_t notif_thread;
    if (pthread_create(&notif_thread, NULL, challenge_simulator, &client_data) != 0) {
        perror("Failed to create notification thread");
        return 1;
    }

    int needs_redraw = 1;

    while (client_data.current_state != STATE_EXIT) {
        
        pthread_mutex_lock(&client_data.data_mutex);
        if (client_data.challenges_updated && client_data.current_state == STATE_HOME) {
            needs_redraw = 1;
            client_data.challenges_updated = 0; //Reset update flag
        }
        pthread_mutex_unlock(&client_data.data_mutex);

        if (needs_redraw) {
            pthread_mutex_lock(&client_data.data_mutex);
            char* output = render_client_state_text(&client_data);
            client_data.status_message[0] = '\0';
            pthread_mutex_unlock(&client_data.data_mutex);
            
            printf("%s", output);
            free(output);
            fflush(stdout);
            needs_redraw = 0;
        }

        pthread_mutex_lock(&client_data.data_mutex);
        int current_state = client_data.current_state;
        pthread_mutex_unlock(&client_data.data_mutex);

        if (current_state == STATE_HOME) {
            // Mode raw pour la navigation
            enable_raw_mode(&orig_termios);
            int key = read_key_with_timeout(50000);
            disable_raw_mode(&orig_termios);
            
            if (key == KEY_NONE) {
             continue; 
            }

            pthread_mutex_lock(&client_data.data_mutex);
            
            if (key == KEY_UP) {
                client_data.selected_menu_option = 
                    (client_data.selected_menu_option - 1 + 9) % 9;
                needs_redraw = 1;
            } 
            else if (key == KEY_DOWN) {
                client_data.selected_menu_option = 
                    (client_data.selected_menu_option + 1) % 9;
                needs_redraw = 1;
            }
            else if (key == KEY_ENTER) {
                switch (client_data.selected_menu_option) {
                    case 0: client_data.current_state = STATE_CHALLENGE; break;
                    case 1: client_data.current_state = STATE_WRITE_BIO; break;
                    case 2: client_data.current_state = STATE_CHOOSE_GAME_SPECTATE; break;
                    case 3: client_data.current_state = STATE_CHOOSE_GAME_TO_REVIEW; break;
                    case 4: client_data.current_state = STATE_VIEW_CHALLENGES; break;
                    case 5: client_data.current_state = STATE_RETRIEVE_BIO; break;
                    case 6: client_data.current_state = STATE_CHOOSE_CHAT; break;
                    case 7: client_data.current_state = STATE_FRIENDS; break;
                    case 8: client_data.current_state = STATE_EXIT; break;
                }
                needs_redraw = 1;
            }
            
            pthread_mutex_unlock(&client_data.data_mutex);
        }

        else if (current_state == STATE_WRITE_BIO) {
            char bio[251];
            fflush(stdout);

            fgets(bio, sizeof(bio), stdin);
            bio[strcspn(bio, "\n")] = '\0';

            if (strcmp(bio, "back") == 0) {
                client_data.current_state = STATE_HOME;
                continue;
            }

            strncpy(client_data.bio, bio, sizeof(client_data.bio));

            char** args = malloc(sizeof(char*) * 1);
            args[0] = malloc(251);
            strncpy(args[0], bio, 251);

            Command* cmd = createCommand("WRITE_BIO", args, 1);
            if (serialize_and_send_Command(socket_fd, cmd) == 0)
                snprintf(client_data.status_message, sizeof(client_data.status_message), "Bio sent successfully ! Your bio is now : %s",client_data.bio);
            else
                snprintf(client_data.status_message, sizeof(client_data.status_message), "Failed to send bio.");

            // back to home
            client_data.current_state = STATE_HOME;
            needs_redraw = 1;
        }

        else if (current_state == STATE_LOGIN) {
            char input[256];
            printf("> ");
            fflush(stdout);
            if (fgets(input, sizeof(input), stdin)) {
                input[strcspn(input, "\n")] = 0;

                if (strcmp(input, "yes") == 0) {
                    // Passe à l'état login username
                    char username[256];
                    char password[256];

                    // Demander l'username
                    printf("Enter username: ");
                    fflush(stdout);
                    if (!fgets(username, sizeof(username), stdin)) {
                        // erreur de lecture
                        continue;
                    }
                    username[strcspn(username, "\n")] = 0;  // enlever '\n'

                    // Demander le mot de passe
                    printf("Enter password: ");
                    fflush(stdout);
                    if (!fgets(password, sizeof(password), stdin)) {
                        // erreur de lecture
                        continue;
                    }
                    password[strcspn(password, "\n")] = 0;

                    // Préparer la commande LOGIN
                    char** args = malloc(sizeof(char*) * 2);

                    args[0] = malloc(251);
                    strncpy(args[0], username, 251);
                    args[1] = malloc(251);
                    strncpy(args[1], password, 251);
                    Command* cmd = createCommand("LOGIN", args, 2);
                    int res = serialize_and_send_Command(socket_fd, cmd);
                    free(args[0]);
                    free(args[1]);

                    pthread_mutex_lock(&client_data.data_mutex);
                    if (res == 0) {
                        // Wait for server confirmation
                        Response *response = receive_and_deserialize_Response(socket_fd);

                        if (response != NULL && response->status_code == 1) {
                            snprintf(client_data.status_message, sizeof(client_data.status_message), "Login successful. Welcome %s!", username);
                            client_data.current_state = STATE_HOME;
                            free(response);
                        } else {
                            snprintf(client_data.status_message, sizeof(client_data.status_message), "Login failed. Try again.");
                            // rester dans STATE_LOGIN pour réessayer
                        }
                    } else {
                        snprintf(client_data.status_message, sizeof(client_data.status_message), "Login failed. Try again.");
                        // rester dans STATE_LOGIN pour réessayer
                    }
                    pthread_mutex_unlock(&client_data.data_mutex);

                    needs_redraw = 1;
                                }
                else if (strcmp(input, "no") == 0) {
                    // Passe à l'inscription
                    client_data.current_state = STATE_SIGN_UP;
                    needs_redraw = 1;
                }
                else {
                    snprintf(client_data.status_message, sizeof(client_data.status_message), "Please answer 'yes' or 'no'");
                    needs_redraw = 1;
                }
            }
        }

        else if (client_data.current_state == STATE_SIGN_UP) {
            char username[256];
            char password[256];

            // Demander l'username
            printf("Create an username: ");
            fflush(stdout);
            if (!fgets(username, sizeof(username), stdin)) {
                // erreur de lecture
                continue;
            }
            username[strcspn(username, "\n")] = 0;  // enlever '\n'

            // Demander le mot de passe
            printf("Create a password: ");
            fflush(stdout);
            if (!fgets(password, sizeof(password), stdin)) {
                // erreur de lecture
                continue;
            }
            password[strcspn(password, "\n")] = 0;

            // Préparer la commande SIGN-UP
            char** args = malloc(sizeof(char*) * 2);

            args[0] = malloc(251);
            strncpy(args[0], username, 251);
            args[1] = malloc(251);
            strncpy(args[1], password, 251);
            Command* cmd = createCommand("SIGNUP", args, 2);
            int res = serialize_and_send_Command(socket_fd, cmd);
            free(args[0]);
            free(args[1]);

            pthread_mutex_lock(&client_data.data_mutex);
            if (res == 0) {
                // Wait for server confirmation
                Response* response = receive_and_deserialize_Response(socket_fd);

                if (response != NULL && response->status_code == 1) {
                    snprintf(client_data.status_message, sizeof(client_data.status_message), "Sign up successful. You can now login %s!", username);
                    client_data.current_state = STATE_LOGIN;
                    free(response);
                } else {
                    snprintf(client_data.status_message, sizeof(client_data.status_message), "Sign up failed. Try again.");
                    // rester dans STATE_LOGIN pour réessayer
                }
            } else {
                snprintf(client_data.status_message, sizeof(client_data.status_message), "Sign up failed. Try again.");
                // rester dans STATE_LOGIN pour réessayer
            }
            pthread_mutex_unlock(&client_data.data_mutex);
            needs_redraw = 1;
        }

        else if (current_state == STATE_RETRIEVE_BIO) {
            char username[256];
            fflush(stdout);

            if (!fgets(username, sizeof(username), stdin)) {
                continue;
            }
            username[strcspn(username, "\n")] = 0; // enlever le '\n'

            // Préparer la commande RETRIEVE_BIO
            char** args = malloc(sizeof(char*) * 1);
            args[0] = (char*) malloc(MAX_ARG_LEN);
            strncpy(args[0], username, MAX_ARG_LEN);

            Command* cmd = createCommand("RETRIEVE_BIO", args, 1);
            int res = serialize_and_send_Command(socket_fd, cmd);
            free(args[0]);
            free(args);

            pthread_mutex_lock(&client_data.data_mutex);
            if (res == 0) {
                // Attendre la réponse du serveur
                Response* response = receive_and_deserialize_Response(socket_fd);

                if (response != NULL) {
                    if (response->status_code == 1 && response->body_size > 0) {
                        snprintf(client_data.status_message, sizeof(client_data.status_message),
                                "Bio of %s:\n%s", username, response->body[0]);
                    } else {
                        snprintf(client_data.status_message, sizeof(client_data.status_message),
                                "User '%s' not found or no bio available.", username);
                    }

                    // Free memory of response
                    for (int i = 0; i < response->body_size; i++)
                        free(response->body[i]);
                    free(response->body);
                    free(response);
                } else {
                    snprintf(client_data.status_message, sizeof(client_data.status_message),
                            "Error: no response from server.");
                }
            } else {
                snprintf(client_data.status_message, sizeof(client_data.status_message),
                        "Failed to send RETRIEVE_BIO command.");
            }

            client_data.current_state = STATE_HOME;
            pthread_mutex_unlock(&client_data.data_mutex);
            needs_redraw = 1;
        }

        else if (current_state == STATE_VIEW_CHALLENGES) {

                // Créer la commande RETRIEVE_CHALLENGE sans argument
                Command* cmd = createCommand("RETRIEVE_CHALLENGE", NULL, 0);
                int res = serialize_and_send_Command(socket_fd, cmd);

                pthread_mutex_lock(&client_data.data_mutex);

                if (res == 0) {
                    // Attendre la réponse du serveur
                    Response* response = receive_and_deserialize_Response(socket_fd);

                    if (response != NULL) {
                        if (response->status_code == 1 && response->body_size > 0) {
                            // Construire la liste à afficher
                            char list_buffer[2048];
                            strcpy(list_buffer, "Current Challenges:\n");

                            for (int i = 0; i < response->body_size; i++) {
                                strcat(list_buffer, "- ");
                                strcat(list_buffer, response->body[i]);
                                strcat(list_buffer, "\n");
                            }

                            snprintf(client_data.status_message, sizeof(client_data.status_message),
                                    "%s", list_buffer);
                        } else if (response->status_code == 1 && response->body_size == 0) {
                            snprintf(client_data.status_message, sizeof(client_data.status_message),
                                    "No challenges available at the moment.");
                        } else {
                            snprintf(client_data.status_message, sizeof(client_data.status_message),
                                    "Failed to retrieve challenges.");
                        }

                        // Libération mémoire de la réponse
                        for (int i = 0; i < response->body_size; i++)
                            free(response->body[i]);
                        free(response);
                    } else {
                        snprintf(client_data.status_message, sizeof(client_data.status_message),
                                "Error: no response from server.");
                    }
                } else {
                    snprintf(client_data.status_message, sizeof(client_data.status_message),
                            "Failed to send RETRIEVE_CHALLENGE command.");
                }

                client_data.current_state = STATE_HOME;
                pthread_mutex_unlock(&client_data.data_mutex);
                needs_redraw = 1;
            }

            else if (current_state == STATE_CHALLENGE) {
                char username[MAX_ARG_LEN];
                char mode[16];

                fflush(stdout);
                if (!fgets(username, sizeof(username), stdin))
                    continue;
                username[strcspn(username, "\n")] = '\0';

                printf("Enter mode (public/private): ");
                fflush(stdout);
                if (!fgets(mode, sizeof(mode), stdin))
                    continue;
                mode[strcspn(mode, "\n")] = '\0';
                
                // Prépare les arguments
                char** args = malloc(sizeof(char*) * 2);
                args[0] = (char*) malloc(MAX_ARG_LEN);
                strncpy(args[0], username, MAX_ARG_LEN);
                args[1] = (char*) malloc(MAX_ARG_LEN);
                strncpy(args[1], mode, MAX_ARG_LEN);

                Command* cmd = createCommand("CHALLENGE", args, 2);
                int res = serialize_and_send_Command(socket_fd, cmd);

                pthread_mutex_lock(&client_data.data_mutex);

                if (res == 0) {
                    // Attente réponse
                    Response* response = receive_and_deserialize_Response(socket_fd);
                    if (response != NULL) {
                        if (response->status_code == 1) {
                            snprintf(client_data.status_message, sizeof(client_data.status_message),
                                    "Challenge sent successfully to %s (%s mode).", username, mode);
                        } else {
                            snprintf(client_data.status_message, sizeof(client_data.status_message),
                                    "Failed to challenge %s.", username);
                        }

                        for (int i = 0; i < response->body_size; i++)
                            free(response->body[i]);
                        free(response);
                    } else {
                        snprintf(client_data.status_message, sizeof(client_data.status_message),
                                "No response from server.");
                    }
                } else {
                    snprintf(client_data.status_message, sizeof(client_data.status_message),
                            "Failed to send challenge command.");
                }

                client_data.current_state = STATE_HOME;
                pthread_mutex_unlock(&client_data.data_mutex);
                needs_redraw = 1;
            }
        
            else if (current_state == STATE_CHOOSE_GAME_SPECTATE) {
                fflush(stdout);

                // Commande sans argument
                Command* cmd = createCommand("RETRIEVE_GAMES", NULL, 0);
                int res = serialize_and_send_Command(socket_fd, cmd);

                pthread_mutex_lock(&client_data.data_mutex);

                if (res == 0) {
                    Response* response = receive_and_deserialize_Response(socket_fd);
                    if (response != NULL) {
                        if (response->status_code == 1 && response->body_size > 0) {
                            snprintf(client_data.status_message, sizeof(client_data.status_message),
                                    "Retrieved games:\n");
                            //Strucuture attendu : [1,[Alice,Bob]],[2,[Imane,Kamel]],[3,[Louis,Joséphine]]

                            char* data = malloc(strlen(response->body[0]) + 1);
                            strncpy(data, response->body[0], strlen(response->body[0]) + 1);    
                            char *token = strtok(data, "]");        // Coupe à chaque fin de bloc "],"
                            client_data.game_count = 0;

                            while (token != NULL) {
                                // Nettoyage : enlève les crochets ou virgules en trop
                                while (*token == '[' || *token == ',' || *token == ' ') token++;

                                // Exemple de token : "1,[Alice,Bob"
                                char *id_str = strtok(token, ",");  // "1"
                                char *players_str = strtok(NULL, ","); // "[Alice,Bob"

                                if (id_str && players_str) {
                                    // Retire les crochets autour des joueurs
                                    players_str = strchr(players_str, '[');
                                    if (players_str) players_str++;
                                    char *closing = strchr(players_str, ']');
                                    if (closing) *closing = '\0';

                                    // Sépare les deux joueurs
                                    char *player1 = strtok(players_str, ",");
                                    char *player2 = strtok(NULL, ",");

                                    client_data.game_ids[client_data.game_count++] = atoi(id_str);

                                    char line[256];
                                    snprintf(line, sizeof(line),
                                            "  - Game %s: %s vs %s\n",
                                            id_str, player1 ? player1 : "?", player2 ? player2 : "?");

                                    strncat(client_data.status_message, line,
                                            sizeof(client_data.status_message) - strlen(client_data.status_message) - 1);
                                }

                                token = strtok(NULL, "]"); // Passe au prochain bloc
                            }

                            free(data);

                             client_data.current_state = STATE_CHOOSE_GAME_SPECTATE;
                        } else {
                            snprintf(client_data.status_message, sizeof(client_data.status_message),
                                    "Failed to retrieve saved games.");
                        }
                    } else {
                        snprintf(client_data.status_message, sizeof(client_data.status_message),
                                "No response from server.");
                    }
                } else {
                    snprintf(client_data.status_message, sizeof(client_data.status_message),
                            "Failed to send retrieve games command.");
                }

                client_data.current_state = STATE_HOME;
                pthread_mutex_unlock(&client_data.data_mutex);
                needs_redraw = 1;
            }
        else if (current_state == STATE_SPECTATING) {
            fflush(stdout);

            printf("%s", client_data.status_message);
            printf("\nEnter the Game ID to spectate: ");
            fflush(stdout);

            int chosen_id = -1;
            scanf("%d", &chosen_id);

            // Vérifie si le Game ID est valide
            int valid = 0;
            for (int i = 0; i < client_data.game_count; i++) {
                if (client_data.game_ids[i] == chosen_id) {
                    valid = 1;
                    break;
                }
            }

            if (!valid) {
                snprintf(client_data.status_message, sizeof(client_data.status_message),
                        "Invalid Game ID selected.");
                client_data.current_state = STATE_HOME;
            } else {
                // Envoie la commande SPECTATE
                char *args[1];
                char id_str[16];
                snprintf(id_str, sizeof(id_str), "%d", chosen_id);
                args[0] = id_str;

                Command* spectate_cmd = createCommand("SPECTATE", args, 1);
                int res = serialize_and_send_Command(socket_fd, spectate_cmd);

                if (res == 0) {
                    Response* response = receive_and_deserialize_Response(socket_fd);
                    if (response != NULL) {
                        if (response->status_code == 1) {
                            snprintf(client_data.status_message, sizeof(client_data.status_message),
                                    "Now spectating game %d...", chosen_id);
                            client_data.current_state = STATE_SPECTATING;
                        } else {
                            snprintf(client_data.status_message, sizeof(client_data.status_message),
                                    "Failed to spectate game %d.", chosen_id);
                            client_data.current_state = STATE_HOME;
                        }

                        for (int i = 0; i < response->body_size; i++)
                            free(response->body[i]);
                        free(response);
                    } else {
                        snprintf(client_data.status_message, sizeof(client_data.status_message),
                                "No response from server for spectate request.");
                        client_data.current_state = STATE_HOME;
                    }
                } else {
                    snprintf(client_data.status_message, sizeof(client_data.status_message),
                            "Failed to send spectate command.");
                    client_data.current_state = STATE_HOME;
                }
            }

            pthread_mutex_lock(&client_data.data_mutex);
            pthread_mutex_unlock(&client_data.data_mutex);
            needs_redraw = 1;
        }

        else if (current_state == STATE_RETRIEVE_SELF_GAMES) {
            fflush(stdout);

            // Commande sans argument
            Command* cmd = createCommand("RETRIEVE_SELF_GAMES", NULL, 0);
            int res = serialize_and_send_Command(socket_fd, cmd);

            pthread_mutex_lock(&client_data.data_mutex);

            if (res == 0) {
                Response* response = receive_and_deserialize_Response(socket_fd);
                if (response != NULL) {
                    if (response->status_code == 1 && response->body_size > 0) {
                        snprintf(client_data.status_message, sizeof(client_data.status_message),
                                "Retrieved your saved games:\n");

                        client_data.game_count = 0;  // compteur local

            for (int i = 0; i < response->body_size; i++) {
                    // Exemple de response->body[i] : "[1,MonJeu]"
                    char* copy = (char*)malloc(strlen(response->body[i]) + 1);
                    strcpy(copy, response->body[i]);
                    // Enlève '[' au début et ']' à la fin
                    char* start = copy;
                    if (*start == '[') start++;
                    char* end = strchr(start, ']');
                    if (end) *end = '\0';

                    // Sépare l'id et le nom sur la virgule
                    char* id_str = strtok(start, ",");
                    char* name_str = strtok(NULL, ",");

                    if (id_str && name_str) {
                        client_data.game_ids[client_data.game_count] = atoi(id_str);

                        // Copie du nom (avec gestion de la taille max)
                        strncpy(client_data.game_names[client_data.game_count], name_str, 1000 - 1);
                        client_data.game_names[client_data.game_count][1000 - 1] = '\0';

                        client_data.game_count++;

                        char line[256];
                        snprintf(line, sizeof(line), "  - Game %s: %s\n", id_str, name_str);
                        strncat(client_data.status_message, line,
                                sizeof(client_data.status_message) - strlen(client_data.status_message) - 1);
                    }
                    free(copy);
                }
                    } else {
                        snprintf(client_data.status_message, sizeof(client_data.status_message),
                                "No saved games found.");
                    }
                    for (int i = 0; i < response->body_size; i++)
                        free(response->body[i]);
                    free(response);
                } else {
                    snprintf(client_data.status_message, sizeof(client_data.status_message),
                            "No response from server.");
                }
            } else {
                snprintf(client_data.status_message, sizeof(client_data.status_message),
                        "Failed to send retrieve self games command.");
            }

            client_data.current_state = STATE_HOME;

            pthread_mutex_unlock(&client_data.data_mutex);
            needs_redraw = 1;
        }

        else {
            // Canonical mode for text input
            printf("Enter command: ");
            fflush(stdout);
            
            char input[256];
            if (fgets(input, sizeof(input), stdin)) {
                input[strcspn(input, "\n")] = 0;
                
                pthread_mutex_lock(&client_data.data_mutex);
                
                if (strcmp(input, "back") == 0) {
                    client_data.current_state = STATE_HOME;
                    client_data.selected_menu_option = 0;
                    needs_redraw = 1;
                }
                else if (strlen(input) > 0) {
                    printf("Command '%s' not recognized.\n", input);
                }
                
                pthread_mutex_unlock(&client_data.data_mutex);
            }
        }
    }

    pthread_mutex_destroy(&client_data.data_mutex);
    pthread_join(notif_thread, NULL);

    disable_alternate_buffer();

    printf("\nClient closed.\n");
    return 0;
}