#include <stdio.h>
#include <stdlib.h>

#include "canonical.h"
#include "input.h"
#include "shell.h"

int main()
{
    Shell *shell;
    shell = init_shell();
    disable_canonical_mode(shell->current_canonical);
    char *input = command_input();
    printf("%s",input);
    restore_initial_mode(shell->current_canonical);
    destroy_shell(shell);
}
