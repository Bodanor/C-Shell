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

extern shell_t *current_shell_prop;

int init_shell(void);
void destroy_shell(void);
void shell_prompt(void);
void put_char_on_screen(const unsigned int c);
void put_string_on_screen(const char *string, int string_size);
#endif
