#include "terminal_utils.h"
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>

void enable_alternate_buffer() {
    printf("\033[?1049h");
    fflush(stdout); //Make sure the command is sent immediately
}

// Desactivate the alternate buffer
void disable_alternate_buffer() {
    printf("\033[?1049l");
    fflush(stdout); // Make sure the command is sent immediately
}