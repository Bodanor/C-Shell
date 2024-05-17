#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>

#include "terminal.h"

/* This will hold the terminal window size as well as the cursor position */
Terminal *term_window;

static void window_resize_handler(int signum);
static void window_resize_handler(int signum)
{
    update_cursor_pos(term_window->current_cursor);
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
    set_signal_handlers();
}
void destroy_terminal(void)
{
    if (term_window != NULL) {

        /* Destroy the cursors */
        destroy_cursor(&term_window->current_cursor);
        destroy_window_size(&term_window->current_window);

        free(term_window);
        term_window = NULL;
    }

}

void put_char_on_screen(const unsigned int c)
{
    putchar(c);
    increment_cursor(term_window->current_window, term_window->current_cursor); 
}
