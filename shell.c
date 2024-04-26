#include "shell.h"
#include "env.h"
#include <stdio.h>
#include <string.h>

shell_t *init_shell(void)
{
    shell_t *temp;

    temp = (shell_t*)malloc(sizeof(shell_t));
    if (temp == NULL)
        return NULL;
    
    if (load_history(&temp->history) == -1)
        temp->history = NULL;
    
    temp->current_line_input = NULL; 
    temp->env = init_env();
    if (temp->env == NULL)
        return NULL;
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
       destroy_env(&(*shell)->env);
       destroy_history(&(*shell)->history);
       free((*shell));
       *shell = NULL;
    }
}
void shell_prompt(env_t *env)
{
    char *trimmed_pwd;

    trimmed_pwd = NULL;

    printf("%s", env->user);
    putchar('@');
    printf("%s", env->hostname);
    putchar(':');

    /* If we are at the home directory */
    if (strcmp(env->curr_wd, env->home_dir) == 0)
        putchar('~');
    
    /* If we are at a relative path from the home directory */
    else if ((trimmed_pwd = trim_str(env->curr_wd, env->home_dir)) != NULL) {
        putchar('~');
        printf("%s", trimmed_pwd);
    }
    else {
        printf("%s", env->curr_wd);
    }

    printf(" $ ");

}
