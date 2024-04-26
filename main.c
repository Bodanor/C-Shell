#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "canonical.h"
#include "input.h"
#include "terminal.h"

int main()
{
    Terminal *current_terminal= init_terminal();

    if (read_input(current_terminal) != -1) {
        
    }
    restore_initial_mode(current_terminal->term_canonical);
    destroy_terminal(current_terminal);

}
