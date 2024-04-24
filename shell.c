#include "shell.h"
#include <stdlib.h>

shell_t *init_shell(void)
{
    shell_t *temp;

    temp = (shell_t*)malloc(sizeof(shell_t));
    if (temp == NULL)
        return NULL;
    temp->current_input = NULL;

    return temp; 

}
