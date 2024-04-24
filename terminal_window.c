#include "terminal_window.h"
#include <stdlib.h>


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
