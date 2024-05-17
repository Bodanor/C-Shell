#ifndef __TERMINAL_WINDOW_H__
#define __TERMINAL_WINDOW_H__


typedef struct term_window_s {
    int max_rows;
    int max_columns;
}term_window_t;

term_window_t *init_window_size(void);
void get_terminal_window_size(term_window_t *term_window);
void destroy_window_size(term_window_t **window_size);
#endif

