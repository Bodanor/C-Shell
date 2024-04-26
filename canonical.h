#ifndef __CANONICAL_H__
#define __CANONICAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

typedef struct canonical_t {
    struct termios initial_saved_can_prop;
    struct termios current_can_prop;
}TerminalCanonical;

TerminalCanonical *save_current_canonical_mode(void);
void disable_canonical_mode(TerminalCanonical *current_canonical);
void restore_initial_mode(TerminalCanonical *current_canonical);
void destroy_canonical(TerminalCanonical **current_canonical);
#endif
