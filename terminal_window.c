#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "terminal_window.h"


term_window_t *init_window_size(void)
{
    term_window_t *temp;

    temp = (term_window_t*)malloc(sizeof(term_window_t));
    if (temp == NULL)
        return NULL;

    get_terminal_window_size(temp);

    return temp;

}
void get_terminal_window_size(term_window_t *term_window)
{
    struct winsize temp;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &temp);
    term_window->max_columns = temp.ws_col;
    term_window->max_rows = temp.ws_row;
}

void destroy_window_size(term_window_t **window_size)
{
    if(*window_size != NULL) {
        free(*window_size);
        *window_size = NULL;
    }

}
