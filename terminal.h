#ifndef __TERMNINAL_WINDOW_H__
#define __TERMNINAL_WINDOW_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>

#include "terminal_window.h"
#include "cursor.h"
#include "canonical.h"

typedef struct terminal_t {
    term_window_t *current_window;
    Cursor *current_cursor;
} Terminal;

//void update_window_size(Terminal *terminal_window);
Terminal *init_terminal(void);
void destroy_terminal(Terminal **current_terminal);
void put_char_on_screen(Terminal *current_terminal, const unsigned int c);
#endif
