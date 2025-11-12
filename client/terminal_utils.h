#ifndef TERMINAL_UTILS_H
#define TERMINAL_UTILS_H

#include <termios.h>
#include <unistd.h>

// Activate raw mode for terminal input
void enable_raw_mode(struct termios* orig_termios);

// Restore original terminal settings
void disable_raw_mode(struct termios* orig_termios);

// Read a single key press
int read_key();

// Read a single key press with a timeout
int read_key_with_timeout(long timeout_us);

void enable_alternate_buffer();

void disable_alternate_buffer();

// Codes for special keys
#define KEY_UP 1000
#define KEY_DOWN 1001
#define KEY_ENTER 10
#define KEY_ESC 27
#define KEY_NONE 0

#endif