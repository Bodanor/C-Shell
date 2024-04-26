#include "shell.h"
#include "history.h"
#include <stdlib.h>

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
void destroy_shell(shell_t **shell)
{
    if (*shell != NULL) {
       if ((*shell)->current_line_input != NULL){
           destroy_line(&(*shell)->current_line_input);
           (*shell)->current_line_input = NULL;
       }
       if ((*shell)->old_line_input != NULL){
           destroy_line(&(*shell)->old_line_input);
           (*shell)->old_line_input = NULL;
       }
       destroy_history(&(*shell)->history);
       free((*shell));
       *shell = NULL;
    }
}
