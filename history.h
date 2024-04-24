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
} history_t ;

history_t *load_history(void);
void destroy_history(history_t *history);

#endif
