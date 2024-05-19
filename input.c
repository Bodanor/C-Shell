#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>
#include <unistd.h>

#include "input.h"
#include "cursor.h"
#include "shell.h"
#include "terminal.h"
#include "util.h"

/* This is the actual current input pointer*/
static char *input_ptr; 

/* This is the currently edited line */
history_line_t *current_line_input;

/* This is a backup line in case the user cycles through history */
static history_line_t *old_line_input;

static void handle_arrow_keys(shell_t *shell);
static void delete_char_on_screen(void);
static void arrow_up(shell_t *shell);
static void arrow_down(shell_t *shell);
static void redraw_after_cursor();

void redraw_after_cursor()
{
    Cursor *backup_cursor;
    int i;
    
    /* We have to backup the cursor as put_char_on_screen increments the cursor
     * as every character is printed
     */ 
    backup_cursor = dup_cursor(term_window->current_cursor);
    
    /* Now, we clear from the cursor position till the end of the screen */
    clear_from_cursor(term_window->current_cursor);

    for (i = 0; i < (int)(current_line_input->line_length  - (input_ptr - 1  - current_line_input->line)); i++)
    {
        put_char_on_screen(input_ptr[i]);

    }
    
    /* As put_char_on_screen increments the cursor, we have to put back
     * the cursor position back to where it was before this function call
     */
    flush_cursor(backup_cursor);
    
    /* We also need to put back the cursor position the actual terminal cursor */
    copy_cursor(backup_cursor, term_window->current_cursor);
    
    /* Don't forget to free the structure */
    destroy_cursor(&backup_cursor);

}

void redraw_from_beginning_cursor()
{
    Cursor *backup_cursor;
    int i;

    clear_from_cursor(term_window->beginning_input_cursor);
    
    /* We update the internal cursor position to the beginning of the line */
    copy_cursor(term_window->beginning_input_cursor, term_window->current_cursor); 

    redraw_input();
}

void redraw_input()
{
    unsigned int i;

    put_string_on_screen(current_line_input->line, current_line_input->line_length);

}
static void arrow_down(shell_t *shell)
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
            history_line_cpy(old_line_input, current_line_input); 
        }
        /* If we are still in the history, we can cycle through the history */
        else { 
            cycle_history_down(shell->history, current_line_input);
        }
        
        redraw_from_beginning_cursor();
        /* Reset the buffer pointer to point to the end of the current line */
        input_ptr = &current_line_input->line[current_line_input->line_length];
    }
}

static void arrow_up(shell_t *shell)
{
    int i;

    /* Every time the user changes from it's input to one of the history,
     * we shall save it's input to reflect the changes
    */

    if (check_index_out_of_bounds(shell->history) == 0) {
        history_line_cpy(current_line_input, old_line_input);
    }

    cycle_history_up(shell->history, current_line_input);
    
    redraw_from_beginning_cursor();
    /* Reset the buffer pointer to point to the end of the current line */
    input_ptr = &current_line_input->line[current_line_input->line_length];
}

void handle_arrow_keys(shell_t *shell)
{
    unsigned char arrow[3];
    history_line_t *current_line;

    current_line = current_line_input;
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
            
            if (input_ptr - current_line->line < current_line->line_length) {
                input_ptr++;
                increment_cursor(term_window->current_window, term_window->current_cursor);
            }

            break;

        case 'D':
            /* Left arrow, check that we are no already at the beginning of the
             * line */
            if (input_ptr - current_line->line != 0 && current_line->line_length > 0) {
                input_ptr--;
                decrement_cursor(term_window->current_window, term_window->current_cursor);
                
            }

            break;

            /* Arrow up */
        case 'A':
                arrow_up(shell);
            break;

            /* Arrow down */
        case 'B':
            arrow_down(shell);
            break;

    }


}

static void delete_char_on_screen()
{
    
    decrement_cursor(term_window->current_window, term_window->current_cursor); /* Decrement from the currrent cursor by 1 */

    printf("\033[0J"); /* Erase from the cursor till the end of the screen */
    
    /* Reflect changes inside the buffer */
    strncpy(input_ptr-1, input_ptr , current_line_input->line_length - (input_ptr- current_line_input->line-1));
    input_ptr--;

    /* Print what is missing from the cursor till the end of the current line */
    printf("%.*s", (int)(current_line_input->line_length  - (input_ptr - current_line_input->line)), input_ptr); 

    (current_line_input->line_length)--;
    flush_cursor(term_window->current_cursor);

}

history_line_t *read_input(shell_t *shell)
{
    unsigned int current_byte;

    current_line_input = NULL;
    old_line_input = NULL;

    /* THIS SHOULD GO INSIDE A FUNCTION LIKE RESET_SHELL-INPUT WHATEVER */
    current_line_input = create_history_line();
    if (current_line_input == NULL)
        return NULL;

    old_line_input = create_history_line();
    if (old_line_input == NULL)
        return NULL;

    /* We are using a buffer pointer because the user can edit the line it is 
     * currently writting (going back, going forward,..)
     *
     * Because we are in RAW mode, ECHO is disabled so have to keep track
     * internally for the buffer ourself, and print characters when needeed
     */

    input_ptr = current_line_input->line;
    
    /* The cursor should have already been initialized, we just therefore have
     * to update their coordinates from here
     */
    update_cursor_pos(term_window->current_cursor);
    copy_cursor(term_window->current_cursor, term_window->beginning_input_cursor);

    /* In raw mode, the enter key returns a carriage return character. I could 
     * have enable '\n' in the termios properties but oh well ..
     */ 
    while(current_line_input->line_length < ARG_MAX -1 && (current_byte = getchar()) != '\r') {
    
        switch(current_byte)
        {
            /* Send by SIGWINCH AKA EOF */
            case -1:
                break;
            /* Handle arrow keys */
            case 27:
                handle_arrow_keys(shell);
                break;

            /* Delete key was pressed. On some terminals, the backspace is
             * translated to 8 or 127
             */        
            case 8:
            case 127:
                /* We verify that there is actually bytes to be deleted */
                if (current_line_input->line_length > 0  && input_ptr - current_line_input->line != 0) {
                    delete_char_on_screen();
                }
                break;

            default:
                
                /* If we are inserting a character at the middle of the user input
                 * We should first print the new character on the screen
                 * Then insert that character inside the buffer
                 * redraw the input from the newly inserted character
                 * and finally increment the pointer
                 */  
                if (input_ptr - current_line_input->line != current_line_input->line_length) {
                    put_char_on_screen(current_byte);
                    insert_byte(input_ptr, current_byte, current_line_input->line_length - (input_ptr - current_line_input->line));
                    input_ptr++;
                    redraw_after_cursor();
                }
                else {
                    /* If we are here, we insert the character at the end of the
                     * buffer and simply copy the character and increment the
                     * buffer pointer
                     */ 
                    put_char_on_screen(current_byte);
                    *input_ptr++ = current_byte;
                }
                
                /* Don't forget to increment the buffer length as well */
                current_line_input->line_length++;
                break;
        }
    };

    /* When the user has finished intering its input, we print a newline,
     * and add a null terminator to its input
    */

    putchar('\n');
    current_line_input->line[current_line_input->line_length + 1] = '\0';

    /* Don't forget to free the backup line when finished */
    destroy_line(&old_line_input);
    return current_line_input;
    
}
