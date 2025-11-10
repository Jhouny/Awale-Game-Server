#ifndef CLIENT_TYPE_H
#define CLIENT_TYPE_H

// Client state machine
typedef enum {
    STATE_LOGIN,
    STATE_HOME,
    STATE_VIEW_CHALLENGES,
    STATE_CHALLENGE,
    STATE_ECRIRE_BIO,
    STATE_EN_JEU,
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
} ClientData;

#endif