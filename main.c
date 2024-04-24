#include <signal.h>
#include <stdio.h>
#include "canonical.h"
#include "input.h"
#include "terminal.h"

int main()
{

    
    Terminal *current_terminal= init_terminal();
    Input *input;

    input = read_input(current_terminal);
    if (input == NULL)
        return 0;
    else {
    printf("%s",input->current_input);
    }
    restore_initial_mode(current_terminal->term_canonical);
    destroy_input(input);
    destroy_terminal(current_terminal);

}
