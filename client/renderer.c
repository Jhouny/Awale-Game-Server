#include "app_statemachine.h"
#include "client.h"

char* render_client_state_text(const ClientData* data) {
    char* buffer = malloc(8192); 
    if (!buffer) return NULL;

    sprintf(buffer, "\033[2J\033[H--- Awale Game Client ---\n");

    switch (data->current_state) {
        case STATE_SIGN_UP:
            strcat(buffer, "SIGN UP\n\nCreate your account.\n");
            break;

        case STATE_LOGIN:
            strcat(buffer, "LOGIN\n\nDo you have an account? (yes/no)\n");
            break;

        case STATE_HOME: {
            strcat(buffer, "HOME\n\nWelcome to the Awale server!\n\n");
            
            const char* options[] = {
                "Launch a challenge",
                "Write Bio",
                "Spectate a game",
                "Review saved games",
                "View challenges",
                "Retrieve Bio",
                "Chat",
                "Friends",
                "Logout"
            };
            
            for (int i = 0; i < 9; i++) {
                char line[512];
                
                // Highlight the selected option
                if (i == data->selected_menu_option) {
                    if (i == 4 && data->incoming_challenges_count > 0) {
                        sprintf(line, "  \033[7m> %s (%d incoming)\033[0m\n", 
                                options[i], data->incoming_challenges_count);
                    } else {
                        sprintf(line, "  \033[7m> %s\033[0m\n", options[i]);
                    }
                } else {
                    if (i == 4 && data->incoming_challenges_count > 0) {
                        sprintf(line, "    %s \033[33m(%d incoming)\033[0m\n", 
                                options[i], data->incoming_challenges_count);
                    } else {
                        sprintf(line, "    %s\n", options[i]);
                    }
                }
                
                strcat(buffer, line);
            }
            
            if (strlen(data->status_message) > 0) {
                strcat(buffer, "\n\033[32m");
                strcat(buffer, data->status_message);
                strcat(buffer, "\033[0m\n");
            }

            strcat(buffer, "\n\033[90mUse ↑/↓ arrows to navigate, Enter to select\033[0m\n");
            
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
            strcat(buffer, "WRITE BIO\n\nEnter your new bio (250 caracters).\nType 'back' to return to home.\n");
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

        case STATE_RETRIEVE_CHATS:
            strcat(buffer, "CHOOSE CHAT\n\nEnter username to chat or 'back' to return to home.\n");
            break;

        case STATE_CHATTING:
            strcat(buffer, "CHATTING\n\nType messages directly or 'back' to close chat.\n");
            break;

        case STATE_RETRIEVE_FRIENDS:
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

    return buffer;
}