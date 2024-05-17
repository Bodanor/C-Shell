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
    history_line_t *input;

    shell = init_shell();
    if (shell == NULL) {
        return -1;
    }
    init_terminal();

    shell_prompt(shell);

    if ((input = read_input(shell) )== NULL) {
        printf("Error !");
    }
    printf("%s", input->line);
    restore_initial_mode(shell->term_canonical);
}
