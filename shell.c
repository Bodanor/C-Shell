#include "shell.h"
#include "history.h"

shell_t *init_shell(void)
{
    shell_t *temp;

    temp = (shell_t*)malloc(sizeof(shell_t));
    if (temp == NULL)
        return NULL;
    
    if (load_history(&temp->history) == -1)
        temp->history = NULL;
    
    temp->current_line_input = NULL; 

    return temp; 

}
