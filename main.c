#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "canonical.h"
#include "env.h"
#include "input.h"
#include "shell.h"
#include "terminal.h"

int main()
{
    shell_t *shell;
    Terminal *current_terminal;

    shell = init_shell();
    if (shell == NULL) {
        return -1;
    }
    current_terminal = init_terminal();
    if (current_terminal == NULL)
        return -1;
    

    shell_prompt(shell);
    
    if (read_input(shell, current_terminal) != -1) {
        
    }
    restore_initial_mode(shell->term_canonical);
    destroy_terminal(&current_terminal);
}
