#ifndef TERMINAL_UTILS_H
#define TERMINAL_UTILS_H

#include <termios.h>
#include <unistd.h>

void enable_alternate_buffer();

void disable_alternate_buffer();

#endif