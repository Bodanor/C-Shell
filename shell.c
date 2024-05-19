#include <stdio.h>
#include <string.h>

#include "shell.h"
#include "cursor.h"
#include "terminal.h"

shell_t *current_shell_prop;

int init_shell(void)
{

    current_shell_prop = (shell_t*)malloc(sizeof(shell_t));
    if (current_shell_prop == NULL)
        return -1;
    

    if (load_history(&current_shell_prop->history) == -1)
        current_shell_prop->history = NULL;
    
    current_shell_prop->env = init_env();
    if (current_shell_prop->env == NULL)
        return -1;
    
    current_shell_prop->term_canonical = save_current_canonical_mode();
    disable_canonical_mode(current_shell_prop->term_canonical);

    return 0; 

}
void destroy_shell(void)
{
    if (current_shell_prop != NULL) {
       
       destroy_env(&current_shell_prop->env);
       destroy_history(&current_shell_prop->history);
       free(current_shell_prop);
       current_shell_prop = NULL;
    }
}
void shell_prompt(void)
{
    char *trimmed_pwd;

    trimmed_pwd = NULL;
    
    /* Save the beginning cursor position before printing out. This will come
     * handy when resizing happens later on
     */
    update_cursor_pos(term_window->beginning_output_cursor); 

    put_string_on_screen(current_shell_prop->env->user, strlen(current_shell_prop->env->user));
    put_char_on_screen('@');
    put_string_on_screen( current_shell_prop->env->hostname,  strlen(current_shell_prop->env->hostname));

    put_char_on_screen(':');

    /* If we are at the home directory */
    if (strcmp(current_shell_prop->env->curr_wd, current_shell_prop->env->home_dir) == 0)
        put_char_on_screen('~');
    
    /* If we are at a relative path from the home directory */
    else if ((trimmed_pwd = trim_str(current_shell_prop->env->curr_wd, current_shell_prop->env->home_dir)) != NULL) {
        put_char_on_screen('~');
        put_string_on_screen(trimmed_pwd, strlen(trimmed_pwd));
    }
    else {
        put_string_on_screen(current_shell_prop->env->curr_wd, strlen(current_shell_prop->env->curr_wd));
    }
    
    put_string_on_screen(" $ ", 3);

}
void put_char_on_screen(const unsigned int c)
{
    putchar(c);
    increment_cursor(term_window->current_window, term_window->current_cursor); 
}
void put_string_on_screen(const char *string, int string_size)
{
    unsigned int i;

    for (i = 0; i < string_size; i++) {
        put_char_on_screen(string[i]);
    }
}
