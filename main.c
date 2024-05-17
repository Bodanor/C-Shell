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
    shell_t *shell;

    shell = init_shell();
    if (shell == NULL) {
        return -1;
    }
    init_terminal();

    shell_prompt(shell);

    if (read_input(shell, term_window) != -1) {

    }
    printf("%s", shell->current_line_input->line);
    restore_initial_mode(shell->term_canonical);
}
