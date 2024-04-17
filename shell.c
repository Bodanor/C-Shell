#include "shell.h"
#include "canonical.h"
#include <stdio.h>
#include <stdlib.h>

Shell *init_shell()
{
    Shell *current_shell;

    current_shell = (Shell*)malloc(sizeof(Shell));
    if (current_shell == NULL) {
        fprintf(stderr, "Fatal Malloc error !\n");
        exit(EXIT_FAILURE);
    }
    
    /* We are saving the initial state of the terminal (canonical mode)
     * before modifiying the terminal properties
    */ 

    current_shell->current_canonical = save_current_canonical_mode();
    return current_shell;
}

void destroy_shell(Shell *shell)
{
    if (shell != NULL) {
        if (shell->current_canonical != NULL) {
            destroy_canonical(shell->current_canonical);
        }
        free(shell);
    }
}
