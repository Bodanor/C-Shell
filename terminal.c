#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>

#include "terminal.h"
#include "cursor.h"
#include "input.h"
#include "shell.h"
#include "terminal_window.h"

/* This will hold the terminal window size as well as the cursor position */
Terminal *term_window;

static void window_resize_handler(int signum);
static void window_resize_handler(int signum)
{
    int i;

    /* Get the new terminal window size */
    get_terminal_window_size(term_window->current_window);

    /* Check that the cursor is outside the new window dimension. If yes,
     * We have to redraw everything. Resizing should happend if the cursor is
     * one column outside the window dimension or if the line number
     * is different from the initial cursor line position
     */
    if (term_window->current_cursor->current_x > term_window->current_window->max_columns || term_window->current_cursor->current_y != term_window->beginning_output_cursor->current_y) {
        
       /* Don't know why bu I came to the conclusion that this loop is better
        * visually when resizing */ 
        for (i = 0; i < (term_window->current_cursor->current_y - term_window->beginning_output_cursor->current_y) * 2; i++) {
           printf("\033[1F");
        } 
        /* Reflect the current cursor position based on the 'virtual' one */
        update_cursor_pos(term_window->current_cursor);
        /* Clear from the cursor till the end of the screen */
        clear_from_cursor(term_window->current_cursor);

        /* Redraw the prompt */
        shell_prompt();

        /* Here we update the beginning of the input cursor position as this
         * is mandatory for using the history cycling.
         */
        copy_cursor(term_window->current_cursor, term_window->beginning_input_cursor);

        /* Redraw the current edited line */
        redraw_input();
    }
}

static void set_signal_handlers(void);

static void set_signal_handlers(void)
{
    /* We don't want to be interrupted by anything except for SIGWINCH */
    struct sigaction action;
    sigset_t signal_set;

    action.sa_handler = &window_resize_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGWINCH, &action, NULL);

    sigfillset(&signal_set);
    sigdelset(&signal_set, SIGWINCH);
    sigprocmask(SIG_SETMASK, &signal_set, NULL);
}

void init_terminal(void)
{
    term_window = (Terminal*)malloc(sizeof(Terminal));
    if (term_window == NULL){
        fprintf(stderr, "Fatal : Malloc error !\n");
        exit(EXIT_FAILURE);
    }

    term_window->current_window = init_window_size();
    term_window->current_cursor = init_cursor();
    term_window->beginning_input_cursor = init_cursor();
    term_window->beginning_output_cursor = init_cursor();
    set_signal_handlers();
}
void destroy_terminal(void)
{
    if (term_window != NULL) {

        /* Destroy the cursors */
        destroy_cursor(&term_window->beginning_output_cursor);
        destroy_cursor(&term_window->beginning_input_cursor);
        destroy_cursor(&term_window->current_cursor);
        destroy_window_size(&term_window->current_window);

        free(term_window);
        term_window = NULL;
    }

}

