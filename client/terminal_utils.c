#include "terminal_utils.h"
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>

void enable_raw_mode(struct termios* orig_termios) {
    tcgetattr(STDIN_FILENO, orig_termios);
    
    struct termios raw = *orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO); // Desactivate canonical mode and echo
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode(struct termios* orig_termios) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, orig_termios);
}

int read_key() {
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1) return -1;
    
    if (c == '\x1b') { // Echapment sequence
        char seq[3];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return KEY_ESC;
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return KEY_ESC;
        
        if (seq[0] == '[') {
            if (seq[1] == 'A') return KEY_UP;
            if (seq[1] == 'B') return KEY_DOWN;
        }
        return KEY_ESC;
    }
    
    return c;
}

int read_key_with_timeout(long timeout_us) {
    struct timeval tv;
    fd_set fds;
    char c;
    
    // Configurer le timeout
    tv.tv_sec = timeout_us / 1000000;
    tv.tv_usec = timeout_us % 1000000;

    // Surveiller l'entrée standard (file descriptor 0)
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    // select() attend que STDIN_FILENO soit prêt ou que le timeout expire
    int ret = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);

    if (ret <= 0) {
        // Timeout expiré (ret=0) ou erreur (ret=-1)
        return KEY_NONE;
    } 
    
    // Si on arrive ici, une entrée est disponible (ret=1). On peut lire.

    // Première lecture : lire le premier caractère
    if (read(STDIN_FILENO, &c, 1) != 1) {
        return KEY_NONE;
    } 
    
    // Logique de décodage des séquences d'échappement (copiée de votre read_key)
    if (c == '\x1b') { // Séquence d'échappement
        char seq[3];
        
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return KEY_ESC;
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return KEY_ESC;
        
        if (seq[0] == '[') {
            if (seq[1] == 'A') return KEY_UP;
            if (seq[1] == 'B') return KEY_DOWN;
        }
        return KEY_ESC;
    }
    
    return c;
}

void enable_alternate_buffer() {
    printf("\033[?1049h");
    fflush(stdout); //Make sure the command is sent immediately
}

// Desactivate the alternate buffer
void disable_alternate_buffer() {
    printf("\033[?1049l");
    fflush(stdout); // Make sure the command is sent immediately
}