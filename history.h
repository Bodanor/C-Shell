#ifndef __HISTORY_H__
#define __HISTORY_H__

#include <stdio.h>
#include <stdlib.h>

#define MAX_HISTORY_LINES 500

typedef struct history_line_s{
    char *line;
    int line_length;
}history_line_t;

typedef struct history_s{
    history_line_t** history_lines;
    unsigned int number_lines;
    unsigned int index;
} history_t ;

int load_history(history_t **history);
history_line_t * create_history_line(void);
void destroy_history(history_t *history);
void cycle_history_up(history_t *history, history_line_t *replace);
void cycle_history_down(history_t *history, history_line_t *replace);
void shift_history(history_t *history);
void print_history(history_t *history);
void add_to_history(history_t *history, history_line_t *line_to_add);
void history_line_cpy(history_line_t *src, history_line_t *dst);
#endif
