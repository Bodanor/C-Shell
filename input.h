#ifndef __INPUT_H__
#define __INPUT_H__

#include "cursor.h"
#include "shell.h"
#include "history.h"

extern history_line_t *current_line_input;
history_line_t *read_input(shell_t *shell);
void redraw_from_beginning_cursor();
void redraw_input();
#endif
