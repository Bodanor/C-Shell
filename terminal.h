#ifndef __TERMNINAL_WINDOW_H__
#define __TERMNINAL_WINDOW_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>

#include "terminal_window.h"
#include "cursor.h"

typedef struct terminal_t {
    term_window_t *current_window;
    Cursor *current_cursor;
} Terminal;

void init_terminal(void);
void destroy_terminal(void);
void put_char_on_screen(const unsigned int c);
#endif
