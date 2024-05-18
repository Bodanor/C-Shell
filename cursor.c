#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cursor.h"

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


Cursor *dup_cursor(Cursor *cursor_to_copy)
{
    Cursor *cursor;

    cursor = (Cursor*)malloc(sizeof(Cursor));
    if (cursor == NULL){
        fprintf(stderr, "Fatal : Malloc error !\n");
        exit(EXIT_FAILURE);
    }

    cursor->current_x = cursor_to_copy->current_x;
    cursor->current_y = cursor_to_copy->current_y;

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
            cursor->current_x = 1;
            cursor->current_y ++;
            set_cursor_pos(cursor, cursor->current_x, cursor->current_y);
    }
    else{
        cursor->current_x++;
    }
    set_cursor_pos(cursor, cursor->current_x, cursor->current_y);
}
void decrement_cursor(term_window_t *terminal_window, Cursor *cursor)
{
    /* If the cursor is at the beggining of a row and there is still a row 
     * available above, place the cursor at the end of the row */
    if (cursor->current_x == 1 && cursor->current_y != 0){
        cursor->current_x = terminal_window->max_columns;
        cursor->current_y --;
    }
    /* Else we simply decrement the cursor */
    else {
        cursor->current_x --;
    }
    /* We always have to update the cursor manually to the screen */
    set_cursor_pos(cursor, cursor->current_x, cursor->current_y);
}

void destroy_cursor(Cursor **cursor)
{
    if (*cursor != NULL){
        free(*cursor);
        *cursor = NULL;
    }
}
static void update_tty_cursor(Cursor *cursor)
{

    printf("\033[%d;%dH", cursor->current_y, cursor->current_x);
}
void flush_cursor(Cursor *cursor)
{
    update_tty_cursor(cursor);

}
void clear_from_cursor(Cursor *beginning_cursor)
{
    update_tty_cursor(beginning_cursor);
    printf("\033[0J");
}
void copy_cursor(Cursor *src, Cursor *dest)
{
    dest->current_x = src->current_x;
    dest->current_y = src->current_y;

}
