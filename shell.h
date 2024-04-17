#ifndef __SHELL_h__
#define __SHELL_h__

#include <stdio.h>
#include <stdlib.h>

#include "canonical.h"

typedef struct shell_t {
    TerminalCanonical *current_canonical;
}Shell;

Shell *init_shell(void);
void destroy_shell(Shell *shell);
#endif
