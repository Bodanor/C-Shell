#include "input.h"
#include "cursor.h"
#include "history.h"
#include "terminal.h"
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void handle_arrow_keys(Terminal *current_terminal,char **buffer_ptr);
static void delete_char_on_screen(Terminal *current_terminal, history_line_t *current_line, char **buffer_ptr);
static void put_char_on_screen(Terminal *current_terminal, const unsigned int c);
static void arrow_up(Terminal *current_terminal, char **buffer_ptr);
static void arrow_down(Terminal *current_terminal, char **buffer_ptr);

static void arrow_down(Terminal *current_terminal, char **buffer_ptr)
{
    int i;
    
   if (current_terminal->current_shell->history->index + 1 >= current_terminal->current_shell->history->number_lines){
       current_terminal->current_shell->history->index = current_terminal->current_shell->history->number_lines;
        history_line_cpy(current_terminal->current_shell->old_line_input, current_terminal->current_shell->current_line_input); 
   }
   else { 
       cycle_history_down(current_terminal->current_shell->history, current_terminal->current_shell->current_line_input);
   }
   clear_from_cursor(current_terminal->beginning_cursor, current_terminal->current_cursor);

   /* The history contains the commands with the \n character, therefore we have to print the command without it */
   for (i = 0; i < current_terminal->current_shell->current_line_input->line_length -1; i++){
       putchar(current_terminal->current_shell->current_line_input->line[i]); 
       increment_cursor(current_terminal->current_window, current_terminal->current_cursor);
   }
   /* Reset the buffer pointer to point to the end of the current line */

   *buffer_ptr = &current_terminal->current_shell->current_line_input->line[current_terminal->current_shell->current_line_input->line_length];
}

static void arrow_up(Terminal *current_terminal, char **buffer_ptr)
{
    int i;

    /* This should be a one shot thingy */
    if (strcmp(current_terminal->current_shell->old_line_input->line, "0XDEADBEEF") == 0) {
        printf("ONE SHOT !");
        history_line_cpy(current_terminal->current_shell->current_line_input, current_terminal->current_shell->old_line_input);
        /* Backup up the user input in case he changes his mind later on */
    }

    cycle_history_up(current_terminal->current_shell->history, current_terminal->current_shell->current_line_input);

    clear_from_cursor(current_terminal->beginning_cursor, current_terminal->current_cursor);
    
    /* The history contains the commands with the \n character, therefore we have to print the command without it */
    for (i = 0; i < current_terminal->current_shell->current_line_input->line_length -1; i++){
        putchar(current_terminal->current_shell->current_line_input->line[i]); 
        increment_cursor(current_terminal->current_window, current_terminal->current_cursor);
    }
    /* Reset the buffer pointer to point to the end of the current line */
    *buffer_ptr = &current_terminal->current_shell->current_line_input->line[current_terminal->current_shell->current_line_input->line_length];
}
static void put_char_on_screen(Terminal *current_terminal, const unsigned int c)
{
        putchar(c);
        increment_cursor(current_terminal->current_window, current_terminal->current_cursor); 

}
static void handle_arrow_keys(Terminal *current_terminal, char **buffer_ptr)
{
    unsigned char arrow[3];
    history_line_t *current_line;

    current_line = current_terminal->current_shell->current_line_input;
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
                /* Increment cursor */
                printf("%s", arrow);
                increment_cursor(current_terminal->current_window, current_terminal->current_cursor);
            }

            break;

        case 'D':
            /* Left arrow, check that we are no already at the beginning of the
             * line */
            if (*buffer_ptr - current_line->line != 0 && current_line->line_length > 0) {
                (*buffer_ptr)--;
                printf("%s", arrow);
                /* Decrement cursor position */
                decrement_cursor(current_terminal->current_window, current_terminal->current_cursor);
            }

            break;

            /* Arrow up */
        case 'A':
                arrow_up(current_terminal, buffer_ptr);
            break;

            /* Arrow down */
        case 'B':
            arrow_down(current_terminal, buffer_ptr);
            break;

    }


}

static void delete_char_on_screen(Terminal *current_terminal, history_line_t *current_line, char **buffer_ptr)
{
    decrement_cursor(current_terminal->current_window, current_terminal->current_cursor);
    printf("\033[0J"); /* Erase the entire screen*/
    
    /* Reflect changes inside the buffer */
    strncpy((*buffer_ptr)-1, *buffer_ptr , current_line->line_length - (*buffer_ptr-current_line->line-1));
    (*buffer_ptr) --;
    printf("%.*s", current_line->line_length  - (*buffer_ptr - current_line->line), *buffer_ptr); 

    (current_line->line_length)--;
    flush_cursor(current_terminal->current_cursor);
}

int read_input(Terminal *current_terminal)
{
    char *line_ptr;
    unsigned int current_byte;
    
    /* Initialize the input and it's backup */    

    /* THIS SHOULD GO INSIDE A FUNCTION LIKE RESET_SHELL-INPUT WHATEVER */
    current_terminal->current_shell->current_line_input = create_history_line();
    if (current_terminal->current_shell->current_line_input == NULL)
        return -1;

    current_terminal->current_shell->old_line_input = create_history_line();
    if (current_terminal->current_shell->old_line_input == NULL)
        return -1;

    strcpy(current_terminal->current_shell->old_line_input->line, "0XDEADBEEF");
    current_terminal->current_shell->old_line_input->line_length =11;
    /* We are using a buffer pointer because the user can edit the line it is 
     * currently writting (going back, going forward,..)
     *
     * Because we are in RAW mode, ECHO is disabled so have to keep track
     * internally for the buffer ourself, and print characters when needeed
     */

    line_ptr = current_terminal->current_shell->current_line_input->line;

    update_cursor_pos(current_terminal->beginning_cursor);

    
    /* In raw mode, the enter key returns a carriage return character. I could 
     * have enable '\n' in the termios properties but oh well ..
     */ 
    while(current_terminal->current_shell->current_line_input->line_length < ARG_MAX -1 && (current_byte = getchar()) != '\r') {
    
        switch(current_byte)
        {
            /* Send by SIGWINCH AKA EOF */
            case -1:
                break;
            /* Handle arrow keys */
            case 27:
                handle_arrow_keys(current_terminal, &line_ptr);
                break;
            /* On some terminals, the backspace is translated to 8 or 127 */        
            case 8:
            case 127:
                if (current_terminal->current_shell->current_line_input->line_length > 0  && line_ptr-current_terminal->current_shell->current_line_input->line != 0) {
                    delete_char_on_screen(current_terminal, current_terminal->current_shell->current_line_input, &line_ptr);
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

                if (line_ptr - current_terminal->current_shell->current_line_input->line != current_terminal->current_shell->current_line_input->line_length) {
                    strncpy(line_ptr +1, line_ptr,current_terminal->current_shell->current_line_input->line_length - (line_ptr-current_terminal->current_shell->current_line_input->line));

                }
                else {
                    put_char_on_screen(current_terminal, current_byte);
                }
                *line_ptr++ = current_byte;
                current_terminal->current_shell->current_line_input->line_length++;
                break;

        }
    };

//
//    while(current_input->input_length < ARG_MAX -1 && (current_byte = getchar()) != '\r') {
//        /* Put getchar here, so that loop doesnt break when buffer is full */
//        switch(current_byte)
//        {
//            /* Send by SIGWINCH AKA EOF */
//            case -1:
//                break;
//            /* Handle arrow keys */
//            case 27:
//                handle_arrow_keys(current_terminal, current_input, &buffer_ptr);
//                break;
//            /* On some terminals, the backspace is translated to 8 or 127 */        
//            case 8:
//            case 127:
//                if (current_input->input_length > 0  && buffer_ptr-current_input->current_input != 0) {
//                    delete_char_on_screen(current_terminal, current_input, &buffer_ptr);
//                }
//                break;
//
//                /* If no processing needed, we copy the character entered
//                 * and increment the buffer pointer
//                 */
//
//            default:
//
//                /* If we insert a character somewhere other than at the end of the
//                 * current line, we have to decay the line from the cursor to cursor + 1
//                 * to make room for the be insterted character, or else we overwritte !!
//                 */
//
//                if (buffer_ptr - current_input->current_input != current_input->input_length) {
//                    strncpy(buffer_ptr +1, buffer_ptr, current_input->input_length - (buffer_ptr-current_input->current_input));
//
//                }
//                else {
//                    put_char_on_screen(current_terminal, current_byte);
//                }
//                *buffer_ptr++ = current_byte;
//                current_input->input_length++;
//                break;
//
//        }
//    };
//
//    /* When the user has finished intering its input, we print a newline,
//     * and add a null terminator to its input
//     */
//
//    putchar('\n');
//    current_input->current_input[current_input->input_length + 1] = '\0';
//    return 0;
return 0;
    
}

