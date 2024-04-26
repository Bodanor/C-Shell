#include "history.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int read_one_line_history(FILE *history_fd, history_t *history);
static void destroy_line(history_line_t *line);
static history_t *create_history();

void shift_history(history_t *history)
{
    history_line_t *top_line;
    int i;

    top_line = history->history_lines[0];
    history->index--;
    for (i = 0; i < history->number_lines-1; i++){
        history->history_lines[i] = history->history_lines[i+1];
    }
    history->number_lines--;
    free(top_line);
    
}
static history_t *create_history()
{
    history_t *temp_history;

    temp_history = (history_t*)malloc(sizeof(history_t));
    if (temp_history == NULL)
        return NULL;

    temp_history->history_lines = NULL;
    temp_history->number_lines = 0;
    temp_history->index = 0; 

    temp_history->history_lines = (history_line_t**)malloc(sizeof(history_line_t*)*MAX_HISTORY_LINES);
    if (temp_history->history_lines == NULL)
        return NULL;

    return temp_history;

}

static void destroy_line(history_line_t *line)
{
    free(line->line);
    free(line);

}
void print_history(history_t *history)
{
    unsigned int i;
    for (i = 0; i < history->number_lines; i++)
        printf(" %d : %s", i + 1, history->history_lines[i]->line);

}
void add_to_history(history_t *history, history_line_t *line_to_add)
{
    if (history->number_lines == MAX_HISTORY_LINES) {
        shift_history(history);
    }
    history->history_lines[history->number_lines++] = line_to_add;
    /* The index should always initially be out of bounds */
    history->index++;
}
int read_one_line_history(FILE *history_fd, history_t *history)
{
    history_line_t *current_line;
    size_t line_max_length;

    current_line = (history_line_t*)malloc(sizeof(history_t));
    if (current_line == NULL)
        return -1;
    
   current_line->line = NULL; 
    current_line->line_length = getline(&current_line->line, &line_max_length, history_fd);
    if (current_line->line_length == -1){
        free(current_line->line);
        free(current_line);
        return -1;
    }
    
    current_line->line = (char*)realloc(current_line->line, sizeof(char)*(current_line->line_length + 1));
    if (current_line->line == NULL)
        return -1;

    add_to_history(history, current_line);

    return 0;

}
 int load_history(history_t **history)
{
    FILE *history_fd;
    int c;
    history_t *temp_history;

    temp_history = create_history();
    if (temp_history == NULL)
        return -1;
    if ((history_fd = fopen("test.txt", "r")) == NULL){
        fprintf(stderr, "No history found !");
        return -2;
    }
    while (temp_history->number_lines < MAX_HISTORY_LINES && read_one_line_history(history_fd, temp_history) != -1)
        ;

    fclose(history_fd);
    *history = temp_history;
    return 0;
}

history_line_t * create_history_line(void)
{
    history_line_t *temp;

    temp = (history_line_t*)malloc(sizeof(history_line_t));
    if (temp == NULL)
        return NULL;

    temp->line = (char*)malloc(sizeof(char)*(ARG_MAX) + 1);
    if (temp->line == NULL)
        return NULL;

    temp->line_length = 0;

    return temp;

}

void destroy_history(history_t *history)
{
    int i;
    for (i = 0; i < history->number_lines; i++)
        destroy_line(history->history_lines[i]);

    free(history->history_lines);
    free(history);
}
int add_command_to_history(history_t **history, history_line_t *command)
{
    if (history == NULL){
        *history = create_history();
        if (history == NULL)
            return -1;
    }
    add_to_history(*history, command);
    
    return 0;
}

void cycle_history_up(history_t *history,history_line_t *replace)
{
    if (history->index != 0){
        history->index--;
    }
    history_line_cpy(history->history_lines[history->index], replace);

}

void cycle_history_down(history_t *history, history_line_t *replace)
{
    if (history->index < history->number_lines)
        history->index++;

    history_line_cpy(history->history_lines[history->index], replace);
    
}

void history_line_cpy(history_line_t *src, history_line_t *dst)
{
    memset(dst->line, 0, dst->line_length);
    strncpy(dst->line, src->line, src->line_length);
    dst->line_length = src->line_length;
}

int check_index_out_of_bounds(history_t *history)
{
    if (history->index == history->number_lines)
        return 0;
    else
        return 1;
}
