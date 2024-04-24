#include <signal.h>
#include <stdio.h>
#include "canonical.h"
#include "history.h"
#include "input.h"
#include "terminal.h"

int main()
{
    
    history_t *hist = load_history();
    
 destroy_history(hist);   
   return 0; 
    Terminal *current_terminal= init_terminal();

    current_terminal->current_shell->current_input = read_input(current_terminal);
    printf("%s",current_terminal->current_shell->current_input->current_input);
    restore_initial_mode(current_terminal->term_canonical);
    destroy_terminal(current_terminal);

}
