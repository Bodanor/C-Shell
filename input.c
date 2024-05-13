#include "input.h"
#include "cursor.h"

static void handle_arrow_keys(shell_t *shell, Terminal *term_window, char **buffer_ptr);
static void delete_char_on_screen(Cursor *begin_cursor, Terminal *current_terminal, history_line_t *current_line, char **buffer_ptr);
static void arrow_up(shell_t *shell, Terminal *term_window, char **buffer_ptr);
static void arrow_down(shell_t *shell, Terminal *term_window, char **buffer_ptr);

static void arrow_down(shell_t *shell, Terminal *term_window, char **buffer_ptr)
{
    int i;
    
    /* Check that user has not began to input any commands before cycling into the history
     * Check also that the index is not out of bounds, else we ignore the user history cycling
    */
    if (check_index_out_of_bounds(shell->history) != 0) {

        /* If the user has edited it's line and wants to get back it's changes
         * we shall verify that we are at the end of the history
        */
        if (shell->history->index  + 1 == shell->history->number_lines){
            shell->history->index = shell->history->number_lines;
            history_line_cpy(shell->old_line_input, shell->current_line_input); 
        }
        /* If we are still in the history, we can cycle through the history */
        else { 
            cycle_history_down(shell->history, shell->current_line_input);
        }

        /* Clear from the cursor to the end of the screen */
        clear_from_cursor(shell->beginning_cursor);


        for (i = 0; i < shell->current_line_input->line_length; i++){
            putchar(shell->current_line_input->line[i]);
        }

        /* Reset the buffer pointer to point to the end of the current line */
        *buffer_ptr = &shell->current_line_input->line[shell->current_line_input->line_length];
    }
}

static void arrow_up(shell_t *shell, Terminal *term_window, char **buffer_ptr)
{
    int i;

    /* Every time the user changes from it's input to one of the history,
     * we shall save it's input to reflect the changes
    */

    if (check_index_out_of_bounds(shell->history) == 0) {
        history_line_cpy(shell->current_line_input, shell->old_line_input);
    }

    cycle_history_up(shell->history, shell->current_line_input);

    /* Clear from the cursor to the end of the screen */
    clear_from_cursor(shell->beginning_cursor);
    
    
    for (i = 0; i < shell->current_line_input->line_length; i++){
        putchar(shell->current_line_input->line[i]);
    }
    /* Reset the buffer pointer to point to the end of the current line */
    *buffer_ptr = &shell->current_line_input->line[shell->current_line_input->line_length];
}

void handle_arrow_keys(shell_t *shell, Terminal *term_window, char **buffer_ptr)
{
    unsigned char arrow[3];
    history_line_t *current_line;

    current_line = shell->current_line_input;
    arrow[0] = 27; /* We are going to print the sequence, so add the missing
                      part from the previous function call */

    /* The arrow key in RAW mode is a sequence of 3 bytes, therefore we are 
     * reading 2 more bytes where the 3rd bytes indicates which arrow was 
     * pressed
     */ 

    read(STDIN_FILENO, &arrow[1], 2);


    switch(arrow[2]) {
        case 'C':
                /* Right arrow, check that we are not trying to go past the end of
                 * the current line */
            
            if (*buffer_ptr - current_line->line < current_line->line_length) {
                (*buffer_ptr)++;
                increment_cursor(term_window->current_window, term_window->current_cursor);
            }

            break;

        case 'D':
            /* Left arrow, check that we are no already at the beginning of the
             * line */
            if (*buffer_ptr - current_line->line != 0 && current_line->line_length > 0) {
                (*buffer_ptr)--;
                decrement_cursor(term_window->current_window, term_window->current_cursor);
                
            }

            break;

            /* Arrow up */
        case 'A':
                arrow_up(shell, term_window, buffer_ptr);
            break;

            /* Arrow down */
        case 'B':
            arrow_down(shell, term_window, buffer_ptr);
            break;

    }


}

static void delete_char_on_screen(Cursor *begin_cursor, Terminal *term_window, history_line_t *current_line, char **buffer_ptr)
{
    
    decrement_cursor(term_window->current_window, term_window->current_cursor); /* Decrement from the currrent cursor by 1 */

    printf("\033[0J"); /* Erase from the cursor till the end of the screen */
    
    /* Reflect changes inside the buffer */
    strncpy((*buffer_ptr)-1, *buffer_ptr , current_line->line_length - (*buffer_ptr-current_line->line-1));
    (*buffer_ptr)--;

    /* Print what is missing from the cursor till the end of the current line */
    printf("%.*s", (int)(current_line->line_length  - (*buffer_ptr - current_line->line)), *buffer_ptr); 

    (current_line->line_length)--;
    flush_cursor(term_window->current_cursor);

}

int read_input(shell_t *shell, Terminal *term_window)
{
    char *line_ptr;
    unsigned int current_byte;
    

    /* THIS SHOULD GO INSIDE A FUNCTION LIKE RESET_SHELL-INPUT WHATEVER */
    shell->current_line_input = create_history_line();
    if (shell->current_line_input == NULL)
        return -1;

    shell->old_line_input = create_history_line();
    if (shell->old_line_input == NULL)
        return -1;

    /* We are using a buffer pointer because the user can edit the line it is 
     * currently writting (going back, going forward,..)
     *
     * Because we are in RAW mode, ECHO is disabled so have to keep track
     * internally for the buffer ourself, and print characters when needeed
     */

    line_ptr = shell->current_line_input->line;

    /* Save the beginning of the cursor, so that redraw becomes easier when needed */    
    update_cursor_pos(shell->beginning_cursor);
    update_cursor_pos(term_window->current_cursor);


    
    /* In raw mode, the enter key returns a carriage return character. I could 
     * have enable '\n' in the termios properties but oh well ..
     */ 
    while(shell->current_line_input->line_length < ARG_MAX -1 && (current_byte = getchar()) != '\r') {
    
        switch(current_byte)
        {
            /* Send by SIGWINCH AKA EOF */
            case -1:
                break;
            /* Handle arrow keys */
            case 27:
                handle_arrow_keys(shell, term_window, &line_ptr);
                break;
            /* On some terminals, the backspace is translated to 8 or 127 */        
            case 8:
            case 127:
                if (shell->current_line_input->line_length > 0  && line_ptr - shell->current_line_input->line != 0) {
                    delete_char_on_screen(shell->beginning_cursor, term_window, shell->current_line_input, &line_ptr);
                }
                break;

                /* If no processing needed, we copy the character entered
                 * and increment the buffer pointer
                 */

            default:

                /* If we insert a character somewhere other than at the end of the
                 * current line, we have to decay the line from the cursor to cursor + 1
                 * to make room for the be insterted character, or else we overwritte !!
                 */

                if (line_ptr - shell->current_line_input->line != shell->current_line_input->line_length) {
                    strncpy(line_ptr +1, line_ptr,shell->current_line_input->line_length - (line_ptr - shell->current_line_input->line));

                }
                else {
                    put_char_on_screen(term_window, current_byte);

                }
                *line_ptr++ = current_byte;
                shell->current_line_input->line_length++;
                break;
        }
    };

    /* When the user has finished intering its input, we print a newline,
     * and add a null terminator to its input
    */

    putchar('\n');
    shell->current_line_input->line[shell->current_line_input->line_length + 1] = '\0';

    /* Don't forget to free the backup line when finished */
    destroy_line(&shell->old_line_input);
    return 0;
    
}

