#ifndef __SHELL_H__
#define __SHELL_H__

#include <stdio.h>
#include <stdlib.h>

#include "history.h"
#include "env.h"
#include "util.h"
#include "canonical.h"
#include "cursor.h"

typedef struct shell_s {
    history_t *history;
    env_t *env;
    TerminalCanonical *term_canonical;

}shell_t;

shell_t *init_shell(void);
void destroy_shell(shell_t **shell);
void shell_prompt(shell_t *shell);
#endif
