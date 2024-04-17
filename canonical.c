#include "canonical.h"
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

struct canonical_t {
    struct termios initial_saved_can_prop;
    struct termios current_can_prop;
};

TerminalCanonical *save_current_canonical_mode(void)
{
    TerminalCanonical *current_canonical;

    current_canonical = malloc(sizeof(struct canonical_t));
    if (current_canonical == NULL){
        fprintf(stderr, "Fatal : Malloc error !\n");
        exit(EXIT_FAILURE);
    }

    /* Save current terminal canonical mode on 2 separate structures */

    tcgetattr(STDIN_FILENO, &current_canonical->initial_saved_can_prop);
    tcgetattr(STDIN_FILENO,&current_canonical->current_can_prop);

    return current_canonical;
}
void disable_canonical_mode(TerminalCanonical *current_canonical)
{
    /* We first detect if the current terminal is a tty, if not we shall abord
     * ASAP
    */

    if (!isatty(STDIN_FILENO)) {
        fprintf(stderr, "Current terminal is not a tty !\nAbording...\n");
        exit(EXIT_FAILURE);
    }
    
    /* Make the terminal RAW that is disable canonical mode, as well as other
     * processing stuff already enabled. Carriage return shall be written
     * when a newline as written to stdout
     */

    cfmakeraw(&current_canonical->current_can_prop);
    current_canonical->current_can_prop.c_oflag |= (ONLCR | OPOST);
    
    /* Apply changes only if all output as been written to files */
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &current_canonical->current_can_prop);
    setvbuf(stdin, NULL, _IONBF, 0);

}
void restore_initial_mode(TerminalCanonical *current_canonical)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &current_canonical->initial_saved_can_prop);
}

void destroy_canonical(TerminalCanonical *current_canonical)
{
    if (current_canonical != NULL)
        free(current_canonical);
}
