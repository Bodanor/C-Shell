#ifndef __INPUT_H__
#define __INPUT_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>

#include "terminal.h"
#include "history.h"

int read_input(Terminal *current_terminal);

#endif
