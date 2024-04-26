#ifndef __SHELL_H__
#define __SHELL_H__

#include <stdio.h>
#include <stdlib.h>

#include "history.h"
#include "env.h"
#include "util.h"

typedef struct shell_s {
    history_t *history;
    history_line_t *current_line_input;
    history_line_t *old_line_input;
    env_t *env;
}shell_t;

shell_t *init_shell(void);
void destroy_shell(shell_t **shell);
void shell_prompt(env_t *env);
#endif
