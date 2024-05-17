#include <stdio.h>
#include "util.h"

char *trim_str(char *src, char *substring)
{
    /* Loop while the substring is not found */
    while (*src != '\0' && *substring != '\0' && *src++ != *substring++)
        ;

    /* From here we can assume that :
     * 1. we are at the end of either strings
     * 2. we begin to compare same strings
    */ 

    if (*src != '\0' && *substring != '\0'){
        /* Loop while the substring is  equal */
        while (*src != '\0' && *substring != '\0' && *src++ == *substring++)
            ;
        
        /* If we are at the end of subtring, that means we have found it */
        if (*substring == '\0'){
            return src;
        }
        else
            return NULL;
        
    }
    else {
        /* The substring could not be found */
        return NULL;
    }

}

void insert_byte(char *position, int byte, int n)
{
    int i;
    int to_be_inserted;
    int save_byte;

    to_be_inserted = byte;
    save_byte = *position;

    for (i = 0; i < n + 1; i++) {
        position[i] = to_be_inserted;
        to_be_inserted = save_byte;
        save_byte = position[i + 1];
    }
}