#ifndef __SHELL_H__
#define __SHELL_H__

#include <stdio.h>
#include <stdlib.h>

#include "history.h"

typedef struct shell_s {
    history_t *history;
}shell_t;

shell_t *init_shell(void);
#endif
