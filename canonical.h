#ifndef __CANONICAL_H__
#define __CANONICAL_H__

#include <termios.h>

typedef struct canonical_t {
    struct termios initial_saved_can_prop;
    struct termios current_can_prop;
}TerminalCanonical;

TerminalCanonical *save_current_canonical_mode(void);
void disable_canonical_mode(TerminalCanonical *canonical);
void restore_initial_mode(TerminalCanonical *canonical);
void destroy_canonical(TerminalCanonical **canonical);
#endif
