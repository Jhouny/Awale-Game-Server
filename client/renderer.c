#include "app_statemachine.h"
#include "client.h"

char* render_client_state_text(const ClientData* data) {
    char* buffer = malloc(4096); 
    if (!buffer) return NULL;

    strcpy(buffer, "--- Awale Game Client ---\n");

    switch (data->current_state) {
        case STATE_LOGIN:
            strcat(buffer, "LOGIN\n\nEnter your username...\nEnter your password...\n");
            break;

        case STATE_HOME: {
            char temp[256];
            if (data->incoming_challenges_count > 0) {
                sprintf(temp, " \033[33m\033[1m(%d incoming)\033[0m",
                        data->incoming_challenges_count);
            } else {
                strcpy(temp, "");
            }

            sprintf(buffer + strlen(buffer),
                "HOME\n\nWelcome to the Awale server!\n\n"
                "  1. Launch a challenge\n"
                "  2. Write Bio\n"
                "  3. Spectate a game\n"
                "  4. Review saved games\n"
                "  5. View challenges%s\n"
                "  6. Retrieve Bio\n"
                "  7. Chat\n"
                "  8. Friends\n"
                "  9. Logout (type 'exit')\n",
                temp);
            break;
        }
        
        case STATE_VIEW_CHALLENGES:
            sprintf(buffer + strlen(buffer),
                "INCOMING CHALLENGES\n\nYou have %d pending challenge(s).\n"
                "Type 'back' to return to home.\n",
                data->incoming_challenges_count);
            break;

        case STATE_CHALLENGE:
            strcat(buffer, "CHALLENGE MENU\n\nEnter username to challenge or 'back' to return to home.\n");
            break;

        case STATE_WRITE_BIO:
            strcat(buffer, "WRITE BIO\n\nEnter your new bio (single line).\nType 'cancel' or 'back' to return to home.\n");
            break;

        case STATE_CHOOSE_GAME_SPECTATE:
            strcat(buffer, "CHOOSE GAME TO SPECTATE\n\nList of active games.\nUse 'select <id>' or 'back' to return to home.\n");
            break;

        case STATE_SPECTATING:
            strcat(buffer, "SPECTATING GAME\n\nYou are watching a game.\nType 'back' to return to home.\n");
            break;

        case STATE_CHOOSE_GAME_TO_REVIEW:
            strcat(buffer, "CHOOSE GAME TO REVIEW\n\nList of past games.\nUse 'review <id>' or 'back' to return to home.\n");
            break;

        case STATE_REVIEWING:
            strcat(buffer, "REVIEWING GAME\n\nCommands: 'next', 'prev' or 'back' to return to home.\n");
            break;

        case STATE_RETRIEVE_BIO:
            strcat(buffer, "RETRIEVE BIO\n\nEnter username or 'back' to return to home.\n");
            break;

        case STATE_CHOOSE_CHAT:
            strcat(buffer, "CHOOSE CHAT\n\nEnter username to chat or 'back' to return to home.\n");
            break;

        case STATE_CHATTING:
            strcat(buffer, "CHATTING\n\nType messages directly or 'back' to close chat.\n");
            break;

        case STATE_FRIENDS:
            strcat(buffer, "FRIENDS LIST\n\nCommands:\n  add <username>\n  remove <username>\n  or 'back' to return to home\n");
            break;

        case STATE_IN_GAME:
            strcat(buffer, "IN GAME\n\nAwale board here (placeholder).\n");
            break;

        case STATE_EXIT:
            strcat(buffer, "EXITING CLIENT...\n");
            break;

        default:
            strcat(buffer, "UNDEFINED STATE.\n");
            break;
    }

    strcat(buffer, "\n-----------------------------------\nEnter your command: ");
    return buffer;
}