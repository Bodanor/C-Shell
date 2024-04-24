#include "terminal.h"
#include "canonical.h"
#include "cursor.h"
#include "terminal_window.h"
#include <bits/types/sigset_t.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

Terminal *terminal_window_signal_ptr = NULL;

static void set_signal_handlers(void);

void window_resize_handler(int signum)
{
   update_cursor_pos(terminal_window_signal_ptr->current_cursor);

}

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



Terminal *init_terminal(void)
{
    Terminal *current_terminal;

    current_terminal = (Terminal*)malloc(sizeof(Terminal));
    if (current_terminal == NULL){
        fprintf(stderr, "Fatal : Malloc error !\n");
        exit(EXIT_FAILURE);
    }
    
    terminal_window_signal_ptr = current_terminal;

    current_terminal->term_canonical = save_current_canonical_mode();

    disable_canonical_mode(current_terminal->term_canonical);

    current_terminal->current_window = init_window_size();

    current_terminal->current_cursor = init_cursor();

    set_signal_handlers();

    return current_terminal;

}
void destroy_terminal(Terminal *current_terminal)
{
    if (current_terminal != NULL) {
        destroy_cursor(current_terminal->current_cursor);
        destroy_canonical(current_terminal->term_canonical);
        free(current_terminal);
    }
}
