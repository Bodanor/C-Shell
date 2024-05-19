#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "canonical.h"
#include "input.h"
#include "shell.h"
#include "terminal.h"


int main()
{
    history_line_t *input;
    
    init_shell();
    init_terminal();

    shell_prompt();

    if ((input = read_input(current_shell_prop) )== NULL) {
        printf("Error !");
    }
    printf("%s", input->line);
    restore_initial_mode(current_shell_prop->term_canonical);
}
