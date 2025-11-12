#include "client.h"
#include "renderer.h"

//Simulate incoming challenges every 10 seconds
void* challenge_simulator(void* arg) {
    ClientData* data = (ClientData*) arg;
    while (data->current_state != STATE_EXIT) {
        sleep(10);
        
        pthread_mutex_lock(&data->data_mutex);
        data->incoming_challenges_count++;
        
        // Afficher SEULEMENT si on est en HOME
        if (data->current_state == STATE_HOME) {
            char* output = render_client_state_text(data);
            printf("\033[2J\033[H%s", output); // Efface et réaffiche
            free(output);
            fflush(stdout);
        }
        pthread_mutex_unlock(&data->data_mutex);
    }
    return NULL;
}

int main() {
    struct termios orig_termios;
    
    ClientData client_data = {
        .current_state = STATE_HOME,
        .incoming_challenges_count = 0,
        .selected_menu_option = 0
    };
    pthread_mutex_init(&client_data.data_mutex, NULL);

    pthread_t notif_thread;
    if (pthread_create(&notif_thread, NULL, challenge_simulator, &client_data) != 0) {
        perror("Failed to create notification thread");
        return 1;
    }

    int needs_redraw = 1;

    while (client_data.current_state != STATE_EXIT) {
        
        if (needs_redraw) {
            pthread_mutex_lock(&client_data.data_mutex);
            char* output = render_client_state_text(&client_data);
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
            int key = read_key();
            disable_raw_mode(&orig_termios);
            
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
        else {
            // Mode canonique pour les autres états
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

    printf("\nClient closed.\n");
    return 0;
}