#ifndef APP_STATEMACHINE_H
#define APP_STATEMACHINE_H

#include <pthread.h>

// Client state machine
typedef enum {
    STATE_LOGIN,
    STATE_SIGN_UP, 
    STATE_HOME,
    STATE_VIEW_CHALLENGES,
    STATE_CHALLENGE,
    STATE_WRITE_BIO,
    STATE_IN_GAME,
    STATE_CHOOSE_GAME_SPECTATE,
    STATE_SPECTATING,
    STATE_CHOOSE_GAME_TO_REVIEW,
    STATE_REVIEWING,
    STATE_RETRIEVE_BIO,
    STATE_CHOOSE_CHAT,
    STATE_CHATTING,
    STATE_FRIENDS,
    STATE_EXIT 
} ClientState;

// Client dynamic data
typedef struct {
    ClientState current_state;
    int incoming_challenges_count;
    int selected_menu_option;
    int challenges_updated;
    pthread_mutex_t data_mutex;
    int socket_fd;
    char status_message[256];
    char bio[251];
} ClientData;

#endif