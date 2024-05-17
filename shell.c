#include <string.h>

#include "shell.h"

shell_t *init_shell(void)
{
    shell_t *temp;

    temp = (shell_t*)malloc(sizeof(shell_t));
    if (temp == NULL)
        return NULL;
    

    if (load_history(&temp->history) == -1)
        temp->history = NULL;
    
    temp->env = init_env();
    if (temp->env == NULL)
        return NULL;
    
    temp->term_canonical = save_current_canonical_mode();
    disable_canonical_mode(temp->term_canonical);

    return temp; 

}
void destroy_shell(shell_t **shell)
{
    if (*shell != NULL) {
       
       destroy_env(&(*shell)->env);
       destroy_history(&(*shell)->history);
       free((*shell));
       *shell = NULL;
    }
}
void shell_prompt(shell_t *shell)
{
    char *trimmed_pwd;

    trimmed_pwd = NULL;

    printf("%s", shell->env->user);
    putchar('@');
    printf("%s", shell->env->hostname);
    putchar(':');

    /* If we are at the home directory */
    if (strcmp(shell->env->curr_wd, shell->env->home_dir) == 0)
        putchar('~');
    
    /* If we are at a relative path from the home directory */
    else if ((trimmed_pwd = trim_str(shell->env->curr_wd, shell->env->home_dir)) != NULL) {
        putchar('~');
        printf("%s", trimmed_pwd);
    }
    else {
        printf("%s", shell->env->curr_wd);
    }

    printf(" $ ");
    

}
