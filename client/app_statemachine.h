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
    STATE_RETRIEVE_SELF_GAMES,
    STATE_CHOOSE_GAME_TO_REVIEW,
    STATE_REVIEWING,
    STATE_RETRIEVE_BIO,
    STATE_RETRIEVE_CHATS, 
    STATE_CHOOSE_CHAT,     
    STATE_CHATTING, 
    STATE_SEND_MSG,
    STATE_CREATE_CHAT, 
    STATE_RETRIEVE_FRIENDS,
    STATE_ADD_FRIEND,
    STATE_REMOVE_FRIEND,
    STATE_EXIT 
} ClientState;

// Client dynamic data
typedef struct {
    ClientState current_state;
    char incoming_challenges[1000][251];
    int incoming_challenges_count;
    int selected_menu_option;
    int challenges_updated;
    pthread_mutex_t data_mutex;
    int socket_fd;

    char username[251];
    char status_message[2048];
    char bio[256];

    int game_ids[1000];
    int game_count;
    char game_names[1000][1000];
    char selected_game_name[1000];

    char chat_usernames[1000][251];
    int chat_count;
    int selected_chat_index;
    char selected_chat_user[251];
    char pending_message[251];

    char friends_usernames[1000][251];
    int friends_count;
} ClientData;

#endif