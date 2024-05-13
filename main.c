#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "canonical.h"
#include "input.h"
#include "shell.h"
#include "terminal.h"

Terminal *term_window;

void window_resize_handler(int signum);
void window_resize_handler(int signum)
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




int main()
{
    set_signal_handlers();
    shell_t *shell;

    shell = init_shell();
    if (shell == NULL) {
        return -1;
    }
    term_window = init_terminal();
    if (term_window == NULL)
        return -1;


    shell_prompt(shell);

    if (read_input(shell, term_window) != -1) {

    }
    printf("%s", shell->current_line_input->line);
    restore_initial_mode(shell->term_canonical);
}
