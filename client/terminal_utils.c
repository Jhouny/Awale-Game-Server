#include "terminal_utils.h"
#include <stdio.h>

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