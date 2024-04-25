#include "cursor.h"
#include "terminal_window.h"
#include <stdio.h>

static void get_cursor_pos(Cursor *cursor);
static void update_tty_cursor(Cursor *cursor);
Cursor *init_cursor(void)
{
    Cursor *cursor;

    cursor = (Cursor*)malloc(sizeof(Cursor));
    if (cursor == NULL){
        fprintf(stderr, "Fatal : Malloc error !\n");
        exit(EXIT_FAILURE);
    }

    update_cursor_pos(cursor);
    return cursor;
}

void update_cursor_pos(Cursor *cursor)
{
    get_cursor_pos(cursor);
}

void get_cursor_pos(Cursor *cursor)
{
    int x;
    int y;
    char buf[30] = {0};
    int ret, i, pow;
    char ch;

    y = 0;
    x = 0;

    write (1, "\033[6n", 4);

    for (i = 0, ch = 0; ch != 'R'; i++)
    {
        ret = read(0, &ch, 1);
        if (!ret){
            printf("Error");
        }
        buf[i] = ch;
    }

    if (i < 2){
        printf("Error");
    }

    for (i -= 2, pow = 1; buf[i] != ';'; i--, pow *=10)
        x = x + (buf[i] - '0') *pow;
    for( i-- , pow = 1; buf[i] != '['; i--, pow *= 10)
        y = y + ( buf[i] - '0' ) * pow;

    cursor->current_x = x;
    cursor->current_y = y;

}

void set_cursor_pos(Cursor *cursor, int x, int y)
{
    cursor->current_x = x;
    cursor->current_y = y;
    update_tty_cursor(cursor);
}

void increment_cursor(term_window_t *terminal_window, Cursor *cursor)
{
    if (cursor->current_x == terminal_window->max_columns){
        if (cursor->current_y == terminal_window->max_rows) {
            cursor->current_x = 0;
            cursor->current_y = 0;
        }
        else {
            /* If the cursor is at the end of the max columns and not at the end of
             * the maximum rows possible, then go to the next row and start from x = 033
             */
            cursor->current_x = 0;
            cursor->current_y += 1;
            set_cursor_pos(cursor, cursor->current_x, cursor->current_y);
        }
    }
    else {
        /* Else just increment the cursor safely */
        cursor->current_x += 1;
    }
}
void decrement_cursor(term_window_t *terminal_window, Cursor *cursor)
{
    /* If the cursor is the beggining of a row and there is still a row 
     * available above, place the cursor at the end of the row */
    if (cursor->current_x == 0 && cursor->current_y != 0){
        cursor->current_x = terminal_window->max_columns;
        cursor->current_y -=1;
    }
    /* Else we simply decrement the cursor */
    else if (cursor->current_x != 0){
        cursor->current_x -=1;
    }
    /* We always have to update the cursor manually to the screen */
    set_cursor_pos(cursor, cursor->current_x, cursor->current_y);
}

void destroy_cursor(Cursor *cursor)
{
    if (cursor != NULL)
        free(cursor);
}
static void update_tty_cursor(Cursor *cursor)
{

    printf("\033[%d;%dH", cursor->current_y, cursor->current_x);
}
void flush_cursor(Cursor *cursor)
{
    update_tty_cursor(cursor);

}
void clear_from_cursor(Cursor *beginning_cursor, Cursor *current_cursor)
{
    update_tty_cursor(beginning_cursor);
    printf("\033[0J");
    current_cursor->current_x = beginning_cursor->current_x;
    current_cursor->current_y = beginning_cursor->current_y;
}
