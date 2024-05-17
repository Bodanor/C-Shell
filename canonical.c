#include "canonical.h"

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
void disable_canonical_mode(TerminalCanonical *canonical)
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

    cfmakeraw(&canonical->current_can_prop);
    canonical->current_can_prop.c_oflag |= (ONLCR | OPOST);
    
    /* Apply changes only if all output as been written to files */
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &canonical->current_can_prop);

    /* This will become handy, as the I/O is directly available
     * Thus, we don't need to keep track of the cursor ourselfs 
    */
    setvbuf(stdin, NULL, _IONBF, (size_t) 0);
    setvbuf(stdout, NULL, _IONBF, (size_t) 0);

}
void restore_initial_mode(TerminalCanonical *canonical)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &canonical->initial_saved_can_prop);
}

void destroy_canonical(TerminalCanonical **current_canonical)
{
    if (*current_canonical != NULL){
        free(*current_canonical);
        *current_canonical = NULL;
    }
}
