#include "terminal.h"
#include "cursor.h"
#include "history.h"

Terminal *terminal_window_signal_ptr = NULL;

static void set_signal_handlers(void);
static void redraw_input(history_line_t *current_input);

static void redraw_input(history_line_t *current_input)
{
    int i;
    
   clear_from_cursor(terminal_window_signal_ptr->beginning_cursor, terminal_window_signal_ptr->current_cursor); 

    for(i = 0; i < current_input->line_length; i++)
        putchar(current_input->line[i]);
}
void window_resize_handler(int signum)
{

   update_cursor_pos(terminal_window_signal_ptr->current_cursor);
   redraw_input(terminal_window_signal_ptr->current_shell->current_line_input);

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

    current_terminal->beginning_cursor = init_cursor();
    
    current_terminal->current_shell = init_shell();
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


