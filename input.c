#include "input.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>


static void handle_arrow_keys(const unsigned long *current_command_length, const char *buffer, char **buffer_ptr);

static void handle_arrow_keys(const unsigned long *current_command_length, const char *buffer, char **buffer_ptr)
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
            
            if (*buffer_ptr - buffer < *current_command_length) {
                (*buffer_ptr)++;
                printf("%s", arrow);
            }

            break;

        case 'D':
            /* Left arrow, check that we are no already at the beginning of the
             * line */
            if (*buffer_ptr - buffer != 0 && *current_command_length > 0) {
                (*buffer_ptr)--;
                printf("%s", arrow);
            }

            break;

    }


}
char *command_input(void)
{
    char *buffer;
    char *buffer_ptr;
    unsigned int current_byte;
    unsigned long current_command_length;
    int i; 

    /* Allocate ARG_MAX bytes to the current buffer */

    buffer = (char*)malloc(sizeof(char)*ARG_MAX);
    if (buffer == NULL) {
        fprintf(stderr, "FATAL : malloc error\n");
        exit(EXIT_FAILURE);
    }
    
    /* We are using a buffer pointer because the user can edit the line it is 
     * currently writting (going back, going forward,..)
     *
     * Because we are in RAW mode, ECHO is disabled so have to keep track
     * internally for the buffer ourself, and print characters when needeed
     */

    buffer_ptr = buffer;
    current_command_length = 0;
    
    /* In raw mode, the enter key returns a carriage return character. I could 
     * have enable '\n' in the termios properties but oh well ..
     */ 

    while(current_command_length < ARG_MAX -1 && (current_byte = getchar()) != '\r') {

        /* Put getchar here, so that loop doesnt break when buffer is full */
        switch(current_byte)
        {
            
            /* Handle arrow keys */
            case 27:
                handle_arrow_keys(&current_command_length, buffer, &buffer_ptr);
                break;
                

            case 127:

                /* Here we handle the delete key. As we have a buffer that reflects
                 * the input, we first have to check if we are not a the beginning
                 * of the input, or else we are out of bounds ! Then we copy from
                 * buffer_ptr to buffer_ptr-1 the rest of the input, effectively
                 * erasing one character by overwritting it. Then we decrement
                 * the buffer_ptr and the length of the buffer.
                 *
                 * Secondly, we have to mirror the changes visualy to the user.
                 * Internally what happens is that we do the same thing as above
                 * that is going back one position from the cursoro then copy 
                 * the rest of the input. But we also have to place the cursor 
                 * exactly where it should go, because at every character written,
                 * the cursor is automatically increased by one, so once we have
                 * copied the input, we have to go back by n characters copied.
                 * 
                 */
                if (current_command_length > 0  && buffer_ptr-buffer != 0) {
                    strncpy(buffer_ptr-1, buffer_ptr, current_command_length - (buffer_ptr-1-buffer));
                    buffer_ptr--;
                    current_command_length--;

                    putchar('\b'); /* Needed as we must start cipying from cursor -1 position */
                    

                    printf("%.*s", (current_command_length  ) - (buffer_ptr-buffer), buffer_ptr);
                    putchar(' ');
                    for (i = 0; i < (current_command_length) - (buffer_ptr-buffer) + 1; i++)
                        putchar('\b');
                }
                break;

                /* If no processing needed, we copy the character pressed
                 * and increment the buffer pointer
                 */

            default:

                /* If we insert a character somewhere other than at the end of the
                 * current line, we have to decay the line from the cursor to cursor + 1
                 * to make room for the be insterted character, or else we overwritte !!
                 */

                if (buffer_ptr - buffer != current_command_length) {
                    strncpy(buffer_ptr +1, buffer_ptr, current_command_length - (buffer_ptr-buffer));

                }
                else {
                    putchar(current_byte);
                }
                *buffer_ptr++ = current_byte;
                current_command_length++;
                break;

        }
    };

    /* When the user has finished intering its input, when print a newline,
     * and add a null terminator to its input
     */

    putchar('\n');
    buffer[current_command_length + 1] = '\0';
    return buffer;
    
}
