#ifndef __SHELL_H__
#define __SHELL_H__

#include <stdio.h>
#include <stdlib.h>

#include "input.h"

typedef struct input_t Input;

typedef struct shell_s {
    Input *current_input;
}shell_t;

shell_t *init_shell(void);
#endif
