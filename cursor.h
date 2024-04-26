#ifndef __CURSOR_H__
#define __CURSOR_H__

#include <stdio.h>
#include <stdlib.h>


#include "terminal_window.h"

typedef struct cursor_t{
    int current_x;
    int current_y; 
}Cursor;

void update_cursor_pos(Cursor *cursor);
void set_cursor_pos(Cursor *cursor, int x, int y);
void increment_cursor(term_window_t *terminal_window, Cursor *cursor);
void decrement_cursor(term_window_t *terminal_window, Cursor *cursor);
void flush_cursor(Cursor *cursor);
Cursor *init_cursor(void);
void clear_from_cursor(Cursor *beginning_cursor, Cursor *current_cursor);
void destroy_cursor(Cursor **cursor);
#endif
