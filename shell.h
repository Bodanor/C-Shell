#ifndef __SHELL_H__
#define __SHELL_H__

#include <stdio.h>
#include <stdlib.h>

#include "input.h"
#include "terminal.h"

typedef struct shell_s {
    Input *current_input;
    Terminal *current_term;
}shell_t;

shell_t *init_shell(void);
#endif
