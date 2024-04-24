#include "input.h"
#include "cursor.h"
#include "shell.h"
#include "terminal.h"
#include <linux/limits.h>
#include <stdlib.h>

static void handle_arrow_keys(Terminal *current_terminal, Input *input, char **buffer_ptr);
static void delete_char_on_screen(Terminal *current_terminal, Input *input, char **buffer_ptr);
static void put_char_on_screen(Terminal *current_terminal, const unsigned int c);


static void put_char_on_screen(Terminal *current_terminal, const unsigned int c)
{
        putchar(c);
        increment_cursor(current_terminal->current_window, current_terminal->current_cursor); 

}
static void handle_arrow_keys(Terminal *current_terminal, Input *input, char **buffer_ptr)
{
    unsigned char arrow[3];
    
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
            
            if (*buffer_ptr - input->current_input < input->input_length) {
                (*buffer_ptr)++;
                /* Increment cursor */
                printf("%s", arrow);
                increment_cursor(current_terminal->current_window, current_terminal->current_cursor);
            }

            break;

        case 'D':
            /* Left arrow, check that we are no already at the beginning of the
             * line */
            if (*buffer_ptr - input->current_input != 0 && input->input_length > 0) {
                (*buffer_ptr)--;
                printf("%s", arrow);
                /* Decrement cursor position */
                decrement_cursor(current_terminal->current_window, current_terminal->current_cursor);
            }

            break;

    }


}

static void delete_char_on_screen(Terminal *current_terminal, Input *input, char **buffer_ptr)
{
    decrement_cursor(current_terminal->current_window, current_terminal->current_cursor);
    printf("\033[0J"); /* Erase the entire screen*/
    
    /* Reflect changes inside the buffer */
    strncpy((*buffer_ptr)-1, *buffer_ptr , input->input_length - (*buffer_ptr-input->current_input-1));
    (*buffer_ptr) --;
    printf("%.*s", input->input_length  - (*buffer_ptr - input->current_input), *buffer_ptr); 

    (input->input_length)--;
    flush_cursor(current_terminal->current_cursor);
}

Input *read_input(Terminal *current_terminal)
{
    Input *current_input;
    char *buffer_ptr;
    unsigned int current_byte;
    /* Allocate ARG_MAX bytes to the current buffer */
    
    current_input = (Input*)malloc(sizeof(Input));
    if (current_input == NULL){
        return NULL;
    } 
    
    current_input->current_input = (char*)malloc(sizeof(char)*ARG_MAX);
    if (current_input->current_input == NULL)
        return NULL;

    current_input->input_length = 0;
    
    buffer_ptr = current_input->current_input;
    /* We are using a buffer pointer because the user can edit the line it is 
     * currently writting (going back, going forward,..)
     *
     * Because we are in RAW mode, ECHO is disabled so have to keep track
     * internally for the buffer ourself, and print characters when needeed
     */
    
    /* In raw mode, the enter key returns a carriage return character. I could 
     * have enable '\n' in the termios properties but oh well ..
     */ 

    while(current_input->input_length < ARG_MAX -1 && (current_byte = getchar()) != '\r') {
        /* Put getchar here, so that loop doesnt break when buffer is full */
        switch(current_byte)
        {
            /* Send by SIGWINCH AKA EOF */
            case -1:
                break;
            /* Handle arrow keys */
            case 27:
                handle_arrow_keys(current_terminal, current_input, &buffer_ptr);
                break;
            /* On some terminals, the backspace is translated to 8 or 127 */        
            case 8:
            case 127:
                if (current_input->input_length > 0  && buffer_ptr-current_input->current_input != 0) {
                    delete_char_on_screen(current_terminal, current_input, &buffer_ptr);
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

                if (buffer_ptr - current_input->current_input != current_input->input_length) {
                    strncpy(buffer_ptr +1, buffer_ptr, current_input->input_length - (buffer_ptr-current_input->current_input));

                }
                else {
                    put_char_on_screen(current_terminal, current_byte);
                }
                *buffer_ptr++ = current_byte;
                current_input->input_length++;
                break;

        }
    };

    /* When the user has finished intering its input, we print a newline,
     * and add a null terminator to its input
     */

    putchar('\n');
    current_input->current_input[current_input->input_length + 1] = '\0';
    return current_input;
    
}


void destroy_input(Input *current_input)
{
    if (current_input != NULL){
        free(current_input->current_input);
        free(current_input);
    }
}
