#include "client.h"
#include "renderer.h"

//Simulate incoming challenges every 10 seconds
void* challenge_simulator(void* arg) {
    ClientData* data = (ClientData*) arg;
    while (data->current_state != STATE_EXIT) {
        sleep(10);
        
        pthread_mutex_lock(&data->data_mutex);
        data->incoming_challenges_count++;
        pthread_mutex_unlock(&data->data_mutex);
        
        // Effacer et réafficher
        printf("\r\033[K\n");
        pthread_mutex_lock(&data->data_mutex);
        char* output = render_client_state_text(data);
        pthread_mutex_unlock(&data->data_mutex);
        
        printf("%s", output);
        free(output);
        fflush(stdout);
    }
    return NULL;
}

int main() {
    ClientData client_data = {
        .current_state = STATE_HOME,
        .incoming_challenges_count = 0
    };
    pthread_mutex_init(&client_data.data_mutex, NULL);

    //Thread to simulate incoming challenges
    pthread_t notif_thread;
    if (pthread_create(&notif_thread, NULL, challenge_simulator, &client_data) != 0) {
        perror("Failed to create notification thread");
        return 1;
    }

    while (client_data.current_state != STATE_EXIT) {
        // Display current state
        char* output = render_client_state_text(&client_data);
        printf("%s", output);
        free(output);

        // User inputs
        char input[256];
        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = 0;

        pthread_mutex_lock(&client_data.data_mutex);
        // State management
        if (strcmp(input, "exit") == 0 || strcmp(input, "9") == 0)
            client_data.current_state = STATE_EXIT;
        else if (strcmp(input, "back") == 0)
            client_data.current_state = STATE_HOME;
        else if (client_data.current_state == STATE_HOME) {
            if (strcmp(input, "1") == 0) client_data.current_state = STATE_CHALLENGE;
            else if (strcmp(input, "2") == 0) client_data.current_state = STATE_WRITE_BIO;
            else if (strcmp(input, "3") == 0) client_data.current_state = STATE_CHOOSE_GAME_SPECTATE;
            else if (strcmp(input, "4") == 0) client_data.current_state = STATE_CHOOSE_GAME_TO_REVIEW;
            else if (strcmp(input, "5") == 0) client_data.current_state = STATE_VIEW_CHALLENGES;
            else if (strcmp(input, "6") == 0) client_data.current_state = STATE_RETRIEVE_BIO;
            else if (strcmp(input, "7") == 0) client_data.current_state = STATE_CHOOSE_CHAT;
            else if (strcmp(input, "8") == 0) client_data.current_state = STATE_FRIENDS;
        }

        pthread_mutex_unlock(&client_data.data_mutex);


        usleep(100000);
    }

    pthread_mutex_destroy(&client_data.data_mutex);
    //Wait for notification thread to finish
    pthread_join(notif_thread, NULL);

    printf("Client closed.\n");
    return 0;
}