#ifndef __INPUT_H__
#define __INPUT_H__

#include <stdio.h>
#include <stdlib.h>
#include <linux/limits.h>
#include <string.h>

#include "terminal.h"

typedef struct terminal_t Terminal;

typedef struct input_t {
    char *current_input;
    unsigned long input_length;
} Input;

Input *read_input(Terminal *current_terminal);
void destroy_input(Input *current_input);

#endif
