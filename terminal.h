#ifndef __TERMNINAL_WINDOW_H__
#define __TERMNINAL_WINDOW_H__

#include "terminal_window.h"
#include "cursor.h"

typedef struct terminal_t {
    term_window_t *current_window;
    Cursor *current_cursor;
    Cursor *beginning_input_cursor;
    Cursor *beginning_output_cursor;
} Terminal;


/* This should be a global variable and should be accessible by other modules */
extern Terminal *term_window;

void init_terminal(void);
void destroy_terminal(void);
#endif
